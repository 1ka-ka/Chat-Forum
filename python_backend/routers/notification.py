from fastapi import APIRouter, Request
from database import get_db_cursor

router = APIRouter(prefix="/api/notifications", tags=["notifications"])


@router.get("")
async def get_notifications(request: Request, page: int = 1, page_size: int = 20):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    if page < 1:
        page = 1
    if page_size < 1 or page_size > 100:
        page_size = 20
    offset = (page - 1) * page_size

    cursor, conn = get_db_cursor()
    try:
        cursor.execute("SELECT COUNT(*) AS total FROM notifications WHERE user_id = %s", (user_id,))
        total = cursor.fetchone()["total"]
        total_pages = (total + page_size - 1) // page_size

        cursor.execute(
            "SELECT n.id, n.type, n.post_id, n.comment_id, n.message, n.is_read, n.created_at, "
            "u.id AS actor_id, u.nickname AS actor_nickname, u.avatar_url AS actor_avatar_url "
            "FROM notifications n LEFT JOIN users u ON n.actor_id = u.id "
            "WHERE n.user_id = %s ORDER BY n.created_at DESC LIMIT %s OFFSET %s",
            (user_id, page_size, offset)
        )
        items = []
        for row in cursor.fetchall():
            row["actor_avatar_url"] = row["actor_avatar_url"] or ""
            items.append(row)
        return {"code": 200, "message": "success", "data": {
            "items": items, "total": total, "page": page, "page_size": page_size, "total_pages": total_pages
        }}
    finally:
        conn.close()


@router.get("/unread_count")
async def get_unread_count(request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    cursor, conn = get_db_cursor()
    try:
        cursor.execute("SELECT COUNT(*) AS count FROM notifications WHERE user_id = %s AND is_read = 0", (user_id,))
        count = cursor.fetchone()["count"]
        return {"code": 200, "message": "success", "data": {"count": count}}
    finally:
        conn.close()


@router.put("/read/{notification_id}")
async def mark_notification_read(notification_id: int, request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    cursor, conn = get_db_cursor()
    try:
        cursor.execute("UPDATE notifications SET is_read = 1 WHERE id = %s AND user_id = %s", (notification_id, user_id))
        conn.commit()
        return {"code": 200, "message": "已标记为已读"}
    finally:
        conn.close()


@router.put("/read_all")
async def mark_all_read(request: Request):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    cursor, conn = get_db_cursor()
    try:
        cursor.execute("UPDATE notifications SET is_read = 1 WHERE user_id = %s AND is_read = 0", (user_id,))
        conn.commit()
        return {"code": 200, "message": "全部已标记为已读"}
    finally:
        conn.close()
