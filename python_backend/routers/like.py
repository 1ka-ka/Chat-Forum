from fastapi import APIRouter, Request
from database import get_db_cursor
from utils.notification import create_notification

router = APIRouter(prefix="/api/posts/{post_id}/like", tags=["like"])


@router.post("")
async def toggle_like(request: Request, post_id: int):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}

    cursor, conn = get_db_cursor()
    try:
        cursor.execute("SELECT id FROM likes WHERE post_id = %s AND user_id = %s", (post_id, user_id))
        existing = cursor.fetchone()

        if existing:
            cursor.execute("DELETE FROM likes WHERE post_id = %s AND user_id = %s", (post_id, user_id))
            cursor.execute("UPDATE posts SET like_count = GREATEST(like_count - 1, 0) WHERE id = %s", (post_id,))
            conn.commit()
            is_liked = False
        else:
            cursor.execute("INSERT INTO likes (post_id, user_id) VALUES (%s, %s)", (post_id, user_id))
            cursor.execute("UPDATE posts SET like_count = like_count + 1 WHERE id = %s", (post_id,))
            conn.commit()
            is_liked = True

            cursor.execute("SELECT p.user_id, p.title, u.nickname FROM posts p LEFT JOIN users u ON p.user_id = u.id WHERE p.id = %s", (post_id,))
            post_row = cursor.fetchone()
            if post_row and post_row["user_id"] != user_id:
                create_notification(
                    user_id=post_row["user_id"],
                    actor_id=user_id,
                    ntype="like",
                    post_id=post_id,
                    message=f"{post_row['nickname']} 赞了你的帖子「{post_row['title'][:30]}」"
                )

        cursor.execute("SELECT like_count FROM posts WHERE id = %s", (post_id,))
        like_count = cursor.fetchone()["like_count"]
        return {"code": 200, "message": "success", "data": {"is_liked": is_liked, "like_count": like_count}}
    finally:
        conn.close()


@router.get("/users")
async def get_like_users(post_id: int):
    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "SELECT u.id, u.nickname, u.avatar_url "
            "FROM likes l LEFT JOIN users u ON l.user_id = u.id "
            "WHERE l.post_id = %s ORDER BY l.created_at DESC",
            (post_id,)
        )
        items = []
        for row in cursor.fetchall():
            row["avatar_url"] = row["avatar_url"] or ""
            items.append(row)
        return {"code": 200, "message": "success", "data": items}
    finally:
        conn.close()
