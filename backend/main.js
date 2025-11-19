const WebSocket = require('ws');

const PORT = process.env.PORT || 8080;
const RATE_LIMIT_MS = 50; // Minimum 50ms between messages
const MAX_CODE_LENGTH = 50;
const MIN_CODE_LENGTH = 1;
const MAX_MESSAGE_SIZE = 1024; // 1KB max message size

const wss = new WebSocket.Server({
  port: PORT,
  maxPayload: MAX_MESSAGE_SIZE
});

const clients = new Map(); // code -> Set of WebSocket clients

wss.on('listening', () => {
  console.log(`🚀 SpaceBuddy Relay Server listening on port ${PORT}`);
  console.log(`📊 Mode: ${process.env.NODE_ENV || 'development'}`);
});

wss.on('connection', (ws) => {
  console.log('✅ Client connected');

  // Initialize rate limiting
  ws.lastMessageTime = 0;
  ws.messageCount = 0;

  ws.on('message', (message) => {
    const messageString = message.toString();

    // Rate limiting
    const now = Date.now();
    if (now - ws.lastMessageTime < RATE_LIMIT_MS) {
      console.warn('⚠️  Rate limit exceeded');
      return;
    }
    ws.lastMessageTime = now;
    ws.messageCount++;

    let parsedMessage;
    try {
      parsedMessage = JSON.parse(messageString);
    } catch (error) {
      console.error('❌ Invalid JSON:', error.message);
      ws.send(JSON.stringify({ error: 'Invalid JSON format' }));
      return;
    }

    // Validate code
    const code = parsedMessage.code || parsedMessage.key;
    if (!code || typeof code !== 'string' ||
      code.length < MIN_CODE_LENGTH ||
      code.length > MAX_CODE_LENGTH) {
      console.warn('⚠️  Invalid code');
      ws.send(JSON.stringify({ error: 'Invalid code' }));
      return;
    }

    // Validate action
    const validActions = ['SPACEPUSHED', 'TEST', 'REGISTER'];
    if (parsedMessage.action && !validActions.includes(parsedMessage.action)) {
      console.warn('⚠️  Invalid action:', parsedMessage.action);
      return;
    }

    // Store client's code
    if (!ws.code) {
      ws.code = code;

      // Add to clients map
      if (!clients.has(code)) {
        clients.set(code, new Set());
      }
      clients.get(code).add(ws);
      console.log(`📝 Client registered with code: ${code.substring(0, 3)}***`);
    }

    // Handle TEST action
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

    // Relay message to other clients with same code
    const codeClients = clients.get(code);
    if (codeClients) {
      let relayCount = 0;
      codeClients.forEach((client) => {
        if (client !== ws && client.readyState === WebSocket.OPEN) {
          client.send(messageString);
          relayCount++;
        }
      });

      if (relayCount > 0) {
        console.log(`📤 Message relayed to ${relayCount} client(s)`);
      }
    }
  });

  ws.on('close', () => {
    console.log('❌ Client disconnected');

    // Remove from clients map
    if (ws.code && clients.has(ws.code)) {
      clients.get(ws.code).delete(ws);
      if (clients.get(ws.code).size === 0) {
        clients.delete(ws.code);
      }
    }
  });

  ws.on('error', (error) => {
    console.error('💥 WebSocket error:', error.message);

    // Remove from clients map
    if (ws.code && clients.has(ws.code)) {
      clients.get(ws.code).delete(ws);
      if (clients.get(ws.code).size === 0) {
        clients.delete(ws.code);
      }
    }
  });
});

// Graceful shutdown
process.on('SIGTERM', () => {
  console.log('🛑 SIGTERM received, closing server...');
  wss.close(() => {
    console.log('✅ Server closed');
    process.exit(0);
  });
});

process.on('SIGINT', () => {
  console.log('\n🛑 SIGINT received, closing server...');
  wss.close(() => {
    console.log('✅ Server closed');
    process.exit(0);
  });
});