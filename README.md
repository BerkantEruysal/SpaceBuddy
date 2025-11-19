# SpaceBuddy

SpaceBuddy is a cross-platform desktop application that synchronizes the Space key press between two computers over a WebSocket relay server.

## Features

- 🔄 Real-time Space key synchronization between two users
- 🔒 Password-protected pairing system
- 🧪 Built-in test mode
- 🎮 Toggle synchronization on/off
- 🖥️ Cross-platform support (Windows, Linux)
- 🔐 Secure WebSocket communication (WSS in production)

## Architecture

### SpaceBuddyApp (Qt/C++ Client)
- Qt 6.8+ desktop application
- Global keyboard hook for Space key detection
- WebSocket client for real-time communication
- QML-based user interface

### Backend (Node.js Relay Server)
- WebSocket relay server
- Rate limiting and input validation
- Support for multiple paired clients
- Lightweight and scalable

## Prerequisites

### Client (SpaceBuddyApp)
- Qt 6.8 or higher
- CMake 3.16+
- C++17 compiler
- **Linux**: `libxtst-dev` for key simulation
  ```bash
  sudo apt-get install libxtst-dev
  ```

### Backend
- Node.js 14+ and npm

## Installation

### Backend Setup

1. Navigate to the backend directory:
   ```bash
   cd backend
   ```

2. Install dependencies:
   ```bash
   npm install
   ```

3. (Optional) Create `.env` file from example:
   ```bash
   cp .env.example .env
   ```

4. Start the server:
   ```bash
   # Development mode
   npm run dev
   
   # Production mode
   npm start
   ```

### Client Setup

1. Navigate to the SpaceBuddyApp directory:
   ```bash
   cd SpaceBuddyApp
   ```

2. Build the application:
   ```bash
   mkdir build && cd build
   cmake ..
   make
   ```

3. Run the application:
   ```bash
   ./appSpaceBuddyApp
   ```

## Usage

1. **Start the backend server** (if not already running)
2. **Launch SpaceBuddyApp** on both computers
3. **Enter the same password** on both clients
4. **Click "Bağlan" (Connect)** on both clients
5. **Press Space** on one computer - it will be triggered on the other!

### Test Mode

Click the **"Test"** button to verify the connection. The server will send back a Space signal after 4 seconds.

### Sync Control

Use **"Senkronizasyonu durdur/başlat"** to temporarily disable/enable synchronization without disconnecting.

## Configuration

### Debug vs Production

The application automatically uses different server URLs based on build type:

- **Debug Build**: `ws://localhost:8080`
- **Release Build**: `wss://your-production-server.com`

To change the production server URL, edit `SpaceBuddyApp/config.h`:

```cpp
static QString getServerUrl() {
#ifdef QT_DEBUG
    return QStringLiteral("ws://localhost:8080");
#else
    return QStringLiteral("wss://your-server.com"); // Change this
#endif
}
```

### Backend Configuration

Environment variables (create `.env` file):

```env
PORT=8080
NODE_ENV=production
```

## Security Features

### Backend
- ✅ Rate limiting (50ms minimum between messages)
- ✅ Input validation (code length, message size)
- ✅ Action whitelist
- ✅ Maximum payload size (1KB)
- ✅ Graceful error handling

### Client
- ✅ Thread-safe WebSocket communication
- ✅ Automatic reconnection handling
- ✅ Input validation
- ✅ Secure WebSocket (WSS) in production

## Deployment

### Backend Deployment (Recommended Platforms)

1. **Render.com** (Free tier available)
   - Auto-deploy from GitHub
   - Free SSL/TLS certificates
   - Environment variables support

2. **Railway.app** (Free tier available)
   - One-click deploy
   - Automatic HTTPS

3. **Heroku** (Paid)
   - Mature platform
   - Easy scaling

### Client Distribution

Build release version:
```bash
cd SpaceBuddyApp/build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

The executable will be in the `build` directory.

## Project Structure

```
SpaceBuddy/
├── SpaceBuddyApp/          # Qt C++ Client
│   ├── main.cpp            # Entry point, global key listener
│   ├── backend.h/cpp       # Backend singleton, business logic
│   ├── socketclient.h/cpp  # WebSocket client
│   ├── config.h            # Configuration (debug/release)
│   ├── Main.qml            # UI
│   └── CMakeLists.txt      # Build configuration
├── backend/                # Node.js Relay Server
│   ├── main.js             # WebSocket server
│   ├── package.json        # Dependencies
│   └── .env.example        # Environment template
└── README.md               # This file
```

## Troubleshooting

### Linux: X11/extensions/XTest.h not found
```bash
sudo apt-get install libxtst-dev
```

### Connection fails
- Check if backend server is running
- Verify firewall settings
- Check server URL in `config.h`

### Space key not simulating
- **Linux**: Ensure `libxtst-dev` is installed
- **Windows**: Run as administrator if needed
- Check if sync is enabled (not paused)

## License

MIT License - feel free to use and modify!

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Author

Built with ❤️ for seamless cross-computer collaboration
