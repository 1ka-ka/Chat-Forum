import pymysql

DB_CONFIG = {
    "host": "127.0.0.1",
    "port": 3306,
    "user": "root",
    "password": "123456",
    "database": "chatforum",
    "charset": "utf8mb4",
    "cursorclass": pymysql.cursors.DictCursor,
}


def get_db_cursor():
    conn = pymysql.connect(**DB_CONFIG)
    cursor = conn.cursor()
    return cursor, conn
