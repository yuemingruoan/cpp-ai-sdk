const express = require('express');
const router = express.Router();

// Messages
router.post('/messages', (req, res) => {
  res.json({
    id: 'msg-123',
    type: 'message',
    role: 'assistant',
    content: [{ type: 'text', text: 'Mock response from Anthropic test server' }],
    model: req.body.model || 'claude-3-opus-20240229',
    stop_reason: 'end_turn',
    usage: { input_tokens: 10, output_tokens: 20 }
  });
});

// Message Batches
router.post('/messages/batches', (req, res) => {
  res.json({
    id: 'msgbatch-123',
    type: 'message_batch',
    processing_status: 'in_progress',
    request_counts: { processing: 1, succeeded: 0, errored: 0, canceled: 0, expired: 0 },
    created_at: new Date().toISOString()
  });
});

router.get('/messages/batches/:batch_id', (req, res) => {
  res.json({
    id: req.params.batch_id,
    type: 'message_batch',
    processing_status: 'ended',
    request_counts: { processing: 0, succeeded: 1, errored: 0, canceled: 0, expired: 0 },
    created_at: new Date().toISOString()
  });
});

router.post('/messages/batches/:batch_id/cancel', (req, res) => {
  res.json({
    id: req.params.batch_id,
    type: 'message_batch',
    processing_status: 'canceling',
    created_at: new Date().toISOString()
  });
});

router.get('/messages/batches', (req, res) => {
  res.json({
    data: [{
      id: 'msgbatch-123',
      type: 'message_batch',
      processing_status: 'ended',
      created_at: new Date().toISOString()
    }]
  });
});

router.get('/messages/batches/:batch_id/results', (req, res) => {
  res.send('Mock batch results');
});

module.exports = router;
