"""核心接口自动化测试：用户、处方创建/发布/查询、状态过滤与权限隔离。"""

from datetime import date, timedelta

TODAY = date.today()

# seed 后固定 id：1 李文(医生) 2 陈华(医生) 3 王芳(患者) 4 赵磊(患者)
DOCTOR_LI, DOCTOR_CHEN = 1, 2
PATIENT_WANG, PATIENT_ZHAO = 3, 4


def payload(**overrides) -> dict:
    body = {
        "doctor_id": DOCTOR_LI,
        "patient_id": PATIENT_WANG,
        "diagnosis": "急性上呼吸道感染",
        "follow_up_date": (TODAY + timedelta(days=7)).isoformat(),
        "notes": "多喝水，注意休息",
        "drugs": [
            {
                "drug_name": "阿莫西林胶囊",
                "spec": "0.25g×24粒",
                "dosage": "每次0.5g，每日3次，口服",
                "days": 5,
                "quantity": "1盒",
            }
        ],
    }
    body.update(overrides)
    return body


# ---------- 用户接口 ----------


def test_list_doctors_returns_seed(client):
    resp = client.get("/api/users", params={"role": "doctor"})
    assert resp.status_code == 200
    users = resp.json()
    assert len(users) == 2
    assert [u["name"] for u in users] == ["李文", "陈华"]
    assert all(u["role"] == "doctor" for u in users)


def test_list_users_invalid_role_422(client):
    assert client.get("/api/users", params={"role": "admin"}).status_code == 422


# ---------- 创建草稿 ----------


def test_create_draft_201(client):
    resp = client.post("/api/prescriptions", json=payload())
    assert resp.status_code == 201
    body = resp.json()
    assert body["status"] == "DRAFT"
    assert body["published_at"] is None
    assert body["doctor_name"] == "李文"
    assert body["patient_name"] == "王芳"
    assert len(body["drugs"]) == 1
    assert body["drugs"][0]["id"] > 0
    assert body["drugs"][0]["sort_order"] == 0


def test_create_draft_missing_field_422(client):
    body = payload()
    del body["diagnosis"]
    assert client.post("/api/prescriptions", json=body).status_code == 422


def test_create_draft_past_follow_up_date_422(client):
    body = payload(follow_up_date=(TODAY - timedelta(days=1)).isoformat())
    assert client.post("/api/prescriptions", json=body).status_code == 422


def test_create_draft_empty_drugs_422(client):
    assert client.post("/api/prescriptions", json=payload(drugs=[])).status_code == 422


def test_create_draft_unknown_doctor_404(client):
    resp = client.post("/api/prescriptions", json=payload(doctor_id=999))
    assert resp.status_code == 404
    assert "医生" in resp.json()["detail"]


def test_create_draft_patient_id_as_doctor_404(client):
    # 医生 id 位置传了患者 id：角色不符
    resp = client.post("/api/prescriptions", json=payload(doctor_id=PATIENT_WANG))
    assert resp.status_code == 404


def test_create_draft_unknown_patient_404(client):
    resp = client.post("/api/prescriptions", json=payload(patient_id=999))
    assert resp.status_code == 404
    assert "患者" in resp.json()["detail"]


# ---------- 发布 ----------


def test_publish_ok_200(client):
    rx_id = client.post("/api/prescriptions", json=payload()).json()["id"]
    resp = client.post(f"/api/prescriptions/{rx_id}/publish")
    assert resp.status_code == 200
    body = resp.json()
    assert body["status"] == "PUBLISHED"
    assert body["published_at"] is not None


def test_publish_twice_409(client):
    rx_id = client.post("/api/prescriptions", json=payload()).json()["id"]
    client.post(f"/api/prescriptions/{rx_id}/publish")
    resp = client.post(f"/api/prescriptions/{rx_id}/publish")
    assert resp.status_code == 409
    assert "重复发布" in resp.json()["detail"]


def test_publish_not_found_404(client):
    resp = client.post("/api/prescriptions/999/publish")
    assert resp.status_code == 404


# ---------- 医生端列表 ----------


def test_doctor_list_shows_own_drafts(client):
    # 医生 A 建 2 张（1 草稿 1 已发布），医生 B 建 1 张
    client.post("/api/prescriptions", json=payload())
    client.post("/api/prescriptions", json=payload(notes=None))
    client.post(
        "/api/prescriptions", json=payload(doctor_id=DOCTOR_CHEN)
    )
    resp = client.get("/api/prescriptions", params={"doctor_id": DOCTOR_LI})
    assert resp.status_code == 200
    mine = resp.json()
    assert len(mine) == 2
    assert {rx["status"] for rx in mine} == {"DRAFT"}  # 两张都未发布
    assert all(rx["doctor_id"] == DOCTOR_LI for rx in mine)


def test_doctor_list_unknown_doctor_404(client):
    resp = client.get("/api/prescriptions", params={"doctor_id": 999})
    assert resp.status_code == 404


# ---------- 患者端列表 ----------


def test_patient_list_only_published(client):
    # 患者名下 1 草稿 + 1 已发布：患者接口只能看到已发布那张
    draft_id = client.post("/api/prescriptions", json=payload()).json()["id"]
    published_id = client.post(
        "/api/prescriptions", json=payload(diagnosis="高血压")
    ).json()["id"]
    client.post(f"/api/prescriptions/{published_id}/publish")

    resp = client.get(f"/api/patients/{PATIENT_WANG}/prescriptions")
    assert resp.status_code == 200
    rxs = resp.json()
    assert [rx["id"] for rx in rxs] == [published_id]
    assert draft_id not in [rx["id"] for rx in rxs]


def test_patient_isolation(client):
    # 王芳与赵磊各有 1 张已发布处方：互不可见
    for pid in (PATIENT_WANG, PATIENT_ZHAO):
        rx_id = client.post(
            "/api/prescriptions", json=payload(patient_id=pid)
        ).json()["id"]
        client.post(f"/api/prescriptions/{rx_id}/publish")

    resp = client.get(f"/api/patients/{PATIENT_WANG}/prescriptions")
    assert resp.status_code == 200
    rxs = resp.json()
    assert len(rxs) == 1
    assert rxs[0]["patient_id"] == PATIENT_WANG
    assert rxs[0]["patient_name"] == "王芳"


def test_patient_not_found_404(client):
    resp = client.get("/api/patients/999/prescriptions")
    assert resp.status_code == 404
