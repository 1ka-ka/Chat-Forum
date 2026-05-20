import re
import os

ALLOWED_IMAGE_EXT = {".jpg", ".jpeg", ".png", ".gif"}
MAX_AVATAR_SIZE = 2 * 1024 * 1024
MAX_IMAGE_SIZE = 5 * 1024 * 1024


def is_valid_image(filename: str) -> bool:
    _, ext = os.path.splitext(filename.lower())
    return ext in ALLOWED_IMAGE_EXT


def make_filename(user_id: int, original: str) -> str:
    import time, random
    ext = os.path.splitext(original)[1]
    return f"{user_id}_{int(time.time()*1000)}_{random.randint(1000,9999)}{ext}"
