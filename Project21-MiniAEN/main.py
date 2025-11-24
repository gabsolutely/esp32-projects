#NOTE* MUST HAVE SPECIFIC FILE STRUCTURE AND DEPENDENCIES, REFER TO README FILE FOR DETAILS
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler
from plyer import notification
from colorama import Fore, Style, init
import datetime
import time
import os
import threading
import subprocess

init(autoreset=True)

LOG_DIR = "logs"
os.makedirs(LOG_DIR, exist_ok=True)

def get_log_path():
    today = datetime.date.today()
    return os.path.join(LOG_DIR, f"log_{today}.txt")

class Watcher(FileSystemEventHandler):
    def __init__(self):
        self.last_event = {"path": None, "time": 0}
        self.event_counts = {"created": 0, "modified": 0, "deleted": 0}

    def log(self, action, path):
        filename = os.path.basename(path)
        folder = os.path.basename(os.path.dirname(path))
        now_str = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        log_message = f"[{now_str}] ({folder}) {action.upper()}: {filename}"

        with open(get_log_path(), "a") as f:
            f.write(log_message + "\n")

        color = {
            "created": Fore.GREEN,
            "modified": Fore.YELLOW,
            "deleted": Fore.RED
        }.get(action, Fore.WHITE)
        print(color + log_message + Style.RESET_ALL)

        # Desktop notification
        # notification.notify(
        #     title=f"Arduino File {action.capitalize()}",
        #     message=filename,
        #     timeout=2
        # )

        if action in self.event_counts:
            self.event_counts[action] += 1

    def on_any_event(self, event):
        if event.is_directory or not event.src_path.endswith(".ino"):
            return

        if any(ext in event.src_path for ext in [".tmp", ".bak", "~"]):
            return

        now = time.time()
        if self.last_event["path"] == event.src_path and now - self.last_event["time"] < 0.5:
            return
        self.last_event = {"path": event.src_path, "time": now}
        self.log(event.event_type, event.src_path)

def open_logs_folder():
    path = os.path.abspath(LOG_DIR)
    subprocess.Popen(f'explorer "{path}"', shell=True)

def watcher_thread(path, handler):
    observer = Observer()
    observer.schedule(handler, path, recursive=True)
    observer.start()

    print(f"Watching folder: {path}")
    print("Press 'O' to open logs folder\n")

    try:
        while True:
            time.sleep(0.5)
    except KeyboardInterrupt:
        observer.stop()
        print("\nWatcher stopped.")
    observer.join()

    summary = f"\nSession ended: {datetime.datetime.now()}\nEvent summary: {handler.event_counts}\n{'-'*40}\n"
    with open(get_log_path(), "a") as f:
        f.write(summary)
    print("Session summary written to log file.")

if __name__ == "__main__":
    default_path = r"C:\Users\user\Documents\Arduino" # change to your default Arduino path
    WORKSPACE = default_path if os.path.exists(default_path) else input("Enter path to Arduino folder: ")

    if not os.path.exists(WORKSPACE):
        print(f"Folder not found: {WORKSPACE}")
    else:
        open_thread = threading.Thread(target=watcher_thread, args=(WORKSPACE, Watcher()))
        open_thread.start()

        try:
            while True:
                cmd = input("Press CTRL+C to stop \n").strip().lower()
                if cmd == "o":
                    open_logs_folder()
        except KeyboardInterrupt:
            print("\nWatcher manually stopped.")