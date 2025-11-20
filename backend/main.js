const WebSocket = require('ws');
const http = require('http'); // Node.js yerleşik modülü (Kurulum gerektirmez)

// Render portu otomatik atar, yoksa 10000 kullanırız
const PORT = process.env.PORT || 10000;
const RATE_LIMIT_MS = 50;
const MAX_CODE_LENGTH = 50;
const MIN_CODE_LENGTH = 1;
const MAX_MESSAGE_SIZE = 1024; // 1KB

// 1. ADIM: Render'ın "Ayakta mısın?" kontrolü için HTTP Sunucusu
const server = http.createServer((req, res) => {
    // Tarayıcıdan girildiğinde veya Render kontrol ettiğinde bu cevap döner
    res.writeHead(200, { 'Content-Type': 'text/plain' });
    res.end('SpaceBuddy Relay Server is Running! (Active)');
});

// 2. ADIM: WebSocket'i bu HTTP sunucusuna bağlıyoruz
const wss = new WebSocket.Server({ 
    server: server,
    maxPayload: MAX_MESSAGE_SIZE 
});

const clients = new Map(); // code -> Set of WebSocket clients

wss.on('connection', (ws) => {
    console.log('✅ Client connected');

    // Rate limiting init
    ws.lastMessageTime = 0;
    ws.messageCount = 0;

    ws.on('message', (message) => {
        const messageString = message.toString();

        // Rate limiting check
        const now = Date.now();
        if (now - ws.lastMessageTime < RATE_LIMIT_MS) {
            // console.warn('⚠️ Rate limit exceeded'); // Log kirliliğini önlemek için kapattım
            return;
        }
        ws.lastMessageTime = now;
        ws.messageCount++;

        let parsedMessage;
        try {
            parsedMessage = JSON.parse(messageString);
        } catch (error) {
            ws.send(JSON.stringify({ error: 'Invalid JSON format' }));
            return;
        }

        // Code validation (code veya key olarak gelebilir)
        const code = parsedMessage.code || parsedMessage.key;

        if (!code || typeof code !== 'string' ||
            code.length < MIN_CODE_LENGTH ||
            code.length > MAX_CODE_LENGTH) {
            ws.send(JSON.stringify({ error: 'Invalid code' }));
            return;
        }

        // Action validation
        const validActions = ['SPACEPUSHED', 'TEST', 'REGISTER'];
        if (parsedMessage.action && !validActions.includes(parsedMessage.action)) {
            return;
        }

        // Store client logic
        if (!ws.code) {
            ws.code = code;
            if (!clients.has(code)) {
                clients.set(code, new Set());
            }
            clients.get(code).add(ws);
            console.log(`📝 Client registered with code: ${code.substring(0, 3)}***`);
        }

        // TEST Action Logic
        if (parsedMessage.action === 'TEST') {
            console.log('🧪 Test request received, responding in 4 seconds...');
            setTimeout(() => {
                if (ws.readyState === WebSocket.OPEN) {
                    const testResponse = JSON.stringify({
                        action: 'SPACEPUSHED',
                        code: code
                    });
                    ws.send(testResponse);
                    console.log('✅ Test response sent');
                }
            }, 4000);
            return;
        }

        // RELAY Logic (Mesajı diğerlerine ilet)
        const codeClients = clients.get(code);
        if (codeClients) {
            codeClients.forEach((client) => {
                // Mesajı gönderen hariç, aynı koda sahip herkese ilet
                if (client !== ws && client.readyState === WebSocket.OPEN) {
                    client.send(messageString);
                }
            });
        }
    });

    // Temizlik İşlemleri
    const cleanup = () => {
        if (ws.code && clients.has(ws.code)) {
            clients.get(ws.code).delete(ws);
            if (clients.get(ws.code).size === 0) {
                clients.delete(ws.code);
            }
        }
    };

    ws.on('close', () => {
        console.log('❌ Client disconnected');
        cleanup();
    });

    ws.on('error', (error) => {
        console.error('💥 WebSocket error:', error.message);
        cleanup();
    });
});

// 3. ADIM: Dinlemeyi başlatıyoruz (0.0.0.0 Render için çok önemli)
server.listen(PORT, '0.0.0.0', () => {
    console.log(`🚀 SpaceBuddy Relay Server listening on port ${PORT}`);
    console.log(`📊 Environment: ${process.env.NODE_ENV || 'development'}`);
});

// Graceful Shutdown
process.on('SIGTERM', () => {
    console.log('🛑 SIGTERM received. Closing server...');
    server.close(() => {
        console.log('✅ Server closed');
        process.exit(0);
    });
});
