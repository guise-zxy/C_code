# -*- coding: utf-8 -*-
"""实验五 · 业务逻辑层单元测试（标准库 unittest，内存数据库）。

运行：
    python3 -m unittest test_tracker_service -v
"""
import unittest
from datetime import date, timedelta
from tracker_service import TrackerStore, ValidationError


class TestCourses(unittest.TestCase):
    def setUp(self):
        self.s = TrackerStore(":memory:")

    def tearDown(self):
        self.s.close()

    def test_add_course(self):
        c = self.s.add_course("数据结构", 20)
        self.assertEqual(c["name"], "数据结构")
        self.assertEqual(c["target_hours"], 20)
        self.assertEqual(c["progress_percent"], 0)
        self.assertFalse(c["completed"])

    def test_invalid_course(self):
        with self.assertRaises(ValidationError):
            self.s.add_course("", 10)
        with self.assertRaises(ValidationError):
            self.s.add_course("X", 0)
        with self.assertRaises(ValidationError):
            self.s.add_course("X", -5)
        with self.assertRaises(ValidationError):
            self.s.add_course("X", "abc")

    def test_progress_calculation(self):
        c = self.s.add_course("算法", 1)        # 目标 1 学时 = 60 分钟
        self.s.add_record(c["id"], 30)           # 学习 30 分钟 -> 50%
        got = self.s.get_course(c["id"])
        self.assertEqual(got["studied_hours"], 0.5)
        self.assertEqual(got["progress_percent"], 50.0)
        self.assertFalse(got["completed"])

    def test_progress_capped_at_100(self):
        c = self.s.add_course("英语", 1)
        self.s.add_record(c["id"], 200)          # 超过目标
        got = self.s.get_course(c["id"])
        self.assertEqual(got["progress_percent"], 100)
        self.assertTrue(got["completed"])

    def test_delete_cascades_records(self):
        c = self.s.add_course("操作系统", 10)
        self.s.add_record(c["id"], 60)
        self.assertTrue(self.s.delete_course(c["id"]))
        self.assertIsNone(self.s.get_course(c["id"]))
        self.assertEqual(self.s.list_records(c["id"]), [])  # 记录被级联删除


class TestRecords(unittest.TestCase):
    def setUp(self):
        self.s = TrackerStore(":memory:")
        self.cid = self.s.add_course("机器学习", 10)["id"]

    def tearDown(self):
        self.s.close()

    def test_add_record(self):
        r = self.s.add_record(self.cid, 45, "看视频")
        self.assertEqual(r["minutes"], 45)
        self.assertEqual(r["note"], "看视频")

    def test_record_for_missing_course(self):
        with self.assertRaises(ValidationError):
            self.s.add_record(9999, 30)

    def test_invalid_minutes(self):
        for bad in [0, -10, 2000, "30", True]:
            with self.assertRaises(ValidationError):
                self.s.add_record(self.cid, bad)


class TestStatsAndReminders(unittest.TestCase):
    def setUp(self):
        self.s = TrackerStore(":memory:")

    def tearDown(self):
        self.s.close()

    def test_overall_stats(self):
        a = self.s.add_course("A", 1)
        self.s.add_course("B", 5)
        self.s.add_record(a["id"], 60)          # A 达标
        stats = self.s.overall_stats()
        self.assertEqual(stats["course_count"], 2)
        self.assertEqual(stats["total_hours"], 1.0)
        self.assertEqual(stats["completed_courses"], 1)

    def test_reminder_never_studied(self):
        self.s.add_course("拖延的课", 10)
        rem = self.s.reminders()
        self.assertEqual(len(rem), 1)
        self.assertIn("尚未开始", rem[0]["reason"])

    def test_reminder_stale(self):
        c = self.s.add_course("好久没学", 10)
        old = (date.today() - timedelta(days=5)).isoformat()
        self.s.add_record(c["id"], 30, study_date=old)
        rem = self.s.reminders(threshold_days=3)
        self.assertEqual(len(rem), 1)
        self.assertIn("5 天", rem[0]["reason"])

    def test_completed_course_not_reminded(self):
        c = self.s.add_course("已完成", 1)
        self.s.add_record(c["id"], 70)          # 达标
        self.assertEqual(self.s.reminders(), [])


if __name__ == "__main__":
    unittest.main(verbosity=2)
