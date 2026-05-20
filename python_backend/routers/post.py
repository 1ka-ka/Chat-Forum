import json
from fastapi import APIRouter, Request, UploadFile, File, Form
from database import get_db_cursor
from utils.file_util import is_valid_image, make_filename, MAX_IMAGE_SIZE
import os

router = APIRouter(prefix="/api/posts", tags=["posts"])


def safe_truncate(text: str, max_bytes: int = 300) -> str:
    truncated = text.encode('utf-8')[:max_bytes].decode('utf-8', errors='ignore')
    if len(truncated) < len(text):
        truncated += "..."
    return truncated


@router.post("")
async def create_post(
    request: Request,
    title: str = Form(default=""),
    content: str = Form(default=""),
    images: list[UploadFile] = File(default=[]),
):
    user_id = getattr(request.state, "user_id", 0)
    if user_id == 0:
        return {"code": 401, "message": "未认证或Token失效"}
    if not title or len(title) > 200:
        return {"code": 400, "message": "标题格式不正确"}
    if not content:
        return {"code": 400, "message": "内容不能为空"}

    image_urls = []
    for img in images:
        if img.filename and is_valid_image(img.filename):
            filename = make_filename(user_id, img.filename)
            upload_dir = "./uploads/images"
            os.makedirs(upload_dir, exist_ok=True)
            filepath = os.path.join(upload_dir, filename)
            img_content = await img.read()
            if len(img_content) <= MAX_IMAGE_SIZE:
                with open(filepath, "wb") as f:
                    f.write(img_content)
                image_urls.append(f"/uploads/images/{filename}")

    cursor, conn = get_db_cursor()
    try:
        imgs_json = json.dumps(image_urls) if image_urls else None
        cursor.execute(
            "INSERT INTO posts (user_id, title, content, image_urls) VALUES (%s, %s, %s, %s)",
            (user_id, title, content, imgs_json)
        )
        conn.commit()
        post_id = cursor.lastrowid

        cursor.execute(
            "SELECT p.id, p.user_id, p.title, p.content, p.image_urls, p.like_count, p.comment_count, p.created_at, "
            "u.nickname, u.avatar_url, "
            "IFNULL((SELECT 1 FROM likes WHERE post_id = p.id AND user_id = %s), 0) AS is_liked "
            "FROM posts p LEFT JOIN users u ON p.user_id = u.id WHERE p.id = %s",
            (user_id, post_id)
        )
        post = cursor.fetchone()
        post["image_urls"] = json.loads(post["image_urls"]) if post["image_urls"] else []
        post["avatar_url"] = post["avatar_url"] or ""
        post["is_liked"] = bool(post["is_liked"])
        return {"code": 200, "message": "success", "data": post}
    finally:
        conn.close()


@router.get("")
async def get_post_list(request: Request, page: int = 1, page_size: int = 20, search: str = ""):
    user_id = getattr(request.state, "user_id", 0)
    if page < 1:
        page = 1
    if page_size < 1 or page_size > 100:
        page_size = 20
    offset = (page - 1) * page_size

    cursor, conn = get_db_cursor()
    try:
        if search and search.strip():
            keyword = f"%{search.strip()}%"
            cursor.execute(
                "SELECT COUNT(*) AS total FROM posts WHERE title LIKE %s",
                (keyword,)
            )
            total = cursor.fetchone()["total"]
            total_pages = (total + page_size - 1) // page_size

            cursor.execute(
                "SELECT p.id, p.user_id, p.title, p.content, p.image_urls, p.like_count, p.comment_count, p.created_at, "
                "u.nickname, u.avatar_url, "
                "IFNULL((SELECT 1 FROM likes WHERE post_id = p.id AND user_id = %s), 0) AS is_liked "
                "FROM posts p LEFT JOIN users u ON p.user_id = u.id "
                "WHERE p.title LIKE %s ORDER BY p.created_at DESC LIMIT %s OFFSET %s",
                (user_id, keyword, page_size, offset)
            )
        else:
            cursor.execute("SELECT COUNT(*) AS total FROM posts")
            total = cursor.fetchone()["total"]
            total_pages = (total + page_size - 1) // page_size

            cursor.execute(
                "SELECT p.id, p.user_id, p.title, p.content, p.image_urls, p.like_count, p.comment_count, p.created_at, "
                "u.nickname, u.avatar_url, "
                "IFNULL((SELECT 1 FROM likes WHERE post_id = p.id AND user_id = %s), 0) AS is_liked "
                "FROM posts p LEFT JOIN users u ON p.user_id = u.id ORDER BY p.created_at DESC LIMIT %s OFFSET %s",
                (user_id, page_size, offset)
            )

        items = []
        for row in cursor.fetchall():
            row["image_urls"] = json.loads(row["image_urls"]) if row["image_urls"] else []
            row["avatar_url"] = row["avatar_url"] or ""
            row["is_liked"] = bool(row["is_liked"])
            row["content"] = safe_truncate(row["content"])
            items.append(row)

        return {"code": 200, "message": "success", "data": {
            "items": items, "total": total, "page": page, "page_size": page_size, "total_pages": total_pages
        }}
    finally:
        conn.close()


@router.get("/{post_id}")
async def get_post_by_id(post_id: int, request: Request):
    user_id = getattr(request.state, "user_id", 0)
    cursor, conn = get_db_cursor()
    try:
        cursor.execute(
            "SELECT p.id, p.user_id, p.title, p.content, p.image_urls, p.like_count, p.comment_count, p.created_at, "
            "u.nickname, u.avatar_url, "
            "IFNULL((SELECT 1 FROM likes WHERE post_id = p.id AND user_id = %s), 0) AS is_liked "
            "FROM posts p LEFT JOIN users u ON p.user_id = u.id WHERE p.id = %s",
            (user_id, post_id)
        )
        post = cursor.fetchone()
        if not post:
            return {"code": 404, "message": "帖子不存在"}
        post["image_urls"] = json.loads(post["image_urls"]) if post["image_urls"] else []
        post["avatar_url"] = post["avatar_url"] or ""
        post["is_liked"] = bool(post["is_liked"])
        return {"code": 200, "message": "success", "data": post}
    finally:
        conn.close()
