from fastapi import APIRouter, Request
from pydantic import BaseModel
from typing import Optional
from database import get_db_cursor
from utils.notification import create_notification

router = APIRouter(prefix="/api/posts/{post_id}/comments", tags=["comments"])


class CommentReq(BaseModel):
    content: str
    parent_comment_id: Optional[int] = None


@router.post("")
async def create_comment(request: Request, post_id: int, req: CommentReq):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    if not req.content or not req.content.strip() or len(req.content) > 1000:
        return {"code": 400, "message": "评论内容格式不正确"}

    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "INSERT INTO comments (post_id, user_id, parent_comment_id, content) VALUES (%s, %s, %s, %s)",
            (post_id, user_id, req.parent_comment_id, req.content.strip())
        )
        comment_id = cursor.lastrowid
        conn.commit()

        cursor.execute("UPDATE posts SET comment_count = comment_count + 1 WHERE id = %s", (post_id,))
        conn.commit()

        cursor.execute(
            "SELECT c.id, c.post_id, c.content, c.user_id, c.parent_comment_id, c.created_at, "
            "u.nickname, u.avatar_url "
            "FROM comments c LEFT JOIN users u ON c.user_id = u.id WHERE c.id = %s",
            (comment_id,)
        )
        comment = cursor.fetchone()
        if not comment:
            return {"code": 500, "message": "评论创建失败"}
        comment["avatar_url"] = comment["avatar_url"] or ""

        cursor.execute("SELECT user_id, title FROM posts WHERE id = %s", (post_id,))
        post_row = cursor.fetchone()

        if req.parent_comment_id:
            cursor.execute("SELECT user_id FROM comments WHERE id = %s", (req.parent_comment_id,))
            parent_row = cursor.fetchone()
            if parent_row and parent_row["user_id"] != user_id:
                actor_nickname = comment["nickname"]
                create_notification(
                    user_id=parent_row["user_id"],
                    actor_id=user_id,
                    ntype="reply",
                    post_id=post_id,
                    comment_id=comment_id,
                    message=f"{actor_nickname} 回复了你的评论"
                )
        else:
            if post_row and post_row["user_id"] != user_id:
                actor_nickname = comment["nickname"]
                create_notification(
                    user_id=post_row["user_id"],
                    actor_id=user_id,
                    ntype="comment",
                    post_id=post_id,
                    comment_id=comment_id,
                    message=f"{actor_nickname} 评论了你的帖子「{post_row['title'][:30]}」"
                )

        return {"code": 200, "message": "评论成功", "data": comment}
    except Exception as e:
        conn.rollback()
        return {"code": 500, "message": f"评论失败: {str(e)}"}
    finally:
        conn.close()


@router.get("")
async def get_comments(post_id: int, page: int = 1, page_size: int = 20):
    if page < 1:
        page = 1
    if page_size < 1 or page_size > 100:
        page_size = 20
    offset = (page - 1) * page_size

    cursor, conn = get_db_cursor()
    try:
        cursor.execute("SELECT COUNT(*) AS total FROM comments WHERE post_id = %s", (post_id,))
        total = cursor.fetchone()["total"]
        total_pages = (total + page_size - 1) // page_size

        cursor.execute(
            "SELECT c.id, c.post_id, c.content, c.user_id, c.parent_comment_id, c.created_at, "
            "u.nickname, u.avatar_url "
            "FROM comments c LEFT JOIN users u ON c.user_id = u.id "
            "WHERE c.post_id = %s ORDER BY c.created_at ASC LIMIT %s OFFSET %s",
            (post_id, page_size, offset)
        )
        items = []
        for row in cursor.fetchall():
            row["avatar_url"] = row["avatar_url"] or ""
            items.append(row)

        return {"code": 200, "message": "success", "data": {
            "items": items, "total": total, "page": page, "page_size": page_size, "total_pages": total_pages
        }}
    finally:
        conn.close()
