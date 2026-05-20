from fastapi import FastAPI, Request, WebSocket
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from fastapi.middleware.cors import CORSMiddleware
from routers import auth, user, post, comment, like, chat, notification
from utils.auth import decode_token
import os

STATIC_DIR = os.path.join(os.path.dirname(__file__), "frontend", "dist")
HAS_FRONTEND = os.path.isfile(os.path.join(STATIC_DIR, "index.html"))

os.makedirs("./uploads/images", exist_ok=True)
os.makedirs("./uploads/avatars", exist_ok=True)
os.makedirs("./logs", exist_ok=True)

app = FastAPI(title="畅谈 API")

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.middleware("http")
async def auth_middleware(request: Request, call_next):
    path = request.url.path
    skip_auth = (
        path in ["/", "/api/health"]
        or path.startswith("/uploads")
        or path.startswith("/docs")
        or path.startswith("/openapi")
        or path == "/api/auth/login"
        or path == "/api/auth/register"
        or path.startswith("/ws")
        or path.startswith("/assets")
    )
    if skip_auth:
        return await call_next(request)

    auth_header = request.headers.get("Authorization", "")
    user_id = 0
    if auth_header.startswith("Bearer "):
        try:
            user_id = decode_token(auth_header[7:])
        except Exception:
            pass

    request.state.user_id = user_id

    response = await call_next(request)
    return response


app.include_router(auth.router)
app.include_router(user.router)
app.include_router(post.router)
app.include_router(comment.router)
app.include_router(like.router)
app.include_router(chat.router)
app.include_router(notification.router)

app.websocket("/ws/chat")(chat.websocket_chat)

app.mount("/uploads", StaticFiles(directory="uploads"), name="uploads")

if HAS_FRONTEND:
    app.mount("/assets", StaticFiles(directory=os.path.join(STATIC_DIR, "assets")), name="assets")


@app.get("/api/health")
async def health():
    return {"status": "ok"}


if HAS_FRONTEND:
    @app.get("/{full_path:path}")
    async def serve_spa(full_path: str):
        file_path = os.path.join(STATIC_DIR, full_path)
        if full_path and os.path.isfile(file_path):
            return FileResponse(file_path)
        return FileResponse(os.path.join(STATIC_DIR, "index.html"))


@app.on_event("startup")
async def startup():
    print("=" * 50)
    print("  畅谈 Server Starting...")
    print(f"  API:       http://0.0.0.0:8080/api")
    print(f"  WebSocket: ws://0.0.0.0:8080/ws/chat")
    if HAS_FRONTEND:
        print(f"  Frontend:  http://0.0.0.0:8080 (single server)")
    else:
        print(f"  Frontend:  http://localhost:5173 (dev server needed)")
        print(f"  Run 'build.bat' to enable single-server mode")
    print("=" * 50)
