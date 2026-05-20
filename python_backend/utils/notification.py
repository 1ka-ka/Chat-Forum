from database import get_db_cursor


def create_notification(user_id: int, actor_id: int, ntype: str, post_id: int = None,
                        comment_id: int = None, message: str = ""):
    if user_id == actor_id:
        return
    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "INSERT INTO notifications (user_id, actor_id, type, post_id, comment_id, message) "
            "VALUES (%s, %s, %s, %s, %s, %s)",
            (user_id, actor_id, ntype, post_id, comment_id, message)
        )
        conn.commit()
    finally:
        conn.close()
