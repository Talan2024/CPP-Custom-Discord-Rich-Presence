# 🎮 C++ Discord Rich Presence

![Banner](https://i.ibb.co/1f6Dc5tW/8188f551-a3f7-4ed0-a7f9-869b07dc15ab.png)

This is a custom **Discord Rich Presence** client built entirely in **C++**, allowing you to set custom game status, images, text, and timestamps directly through Discord IPC.

---

## 💻 Preview

Here’s what the Rich Presence looks like in action on Discord:

![Rich Presence Preview](https://i.ibb.co/hRbwqmph/image.png)

---

## ⚙️ Features

- Connects to Discord's IPC pipe
- Sends custom `SET_ACTIVITY` payloads
- Displays:
  - name
  - Custom state & details
  - Timestamps
  - Custom image 
- Clean native C++ implementation (no external libraries)

---

## 🧠 How It Works

- Opens Discord IPC pipe at `\\?\pipe\discord-ipc-0`
- Performs a `HANDSHAKE` to initialize the connection
- Sends `IDENTIFY` and `SET_ACTIVITY` JSON packets with:
  - Opcode headers
  - Payload size
  - Rich Presence data

---

## 🧪 To-Do

- [ ] Add small image support  
- [ ] GUI interface for editing presence live  
- [ ] Minimize to tray  
- [ ] Add buttons to Rich Presence  
