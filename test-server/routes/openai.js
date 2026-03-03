const express = require('express');
const router = express.Router();

// Chat Completions
router.post('/chat/completions', (req, res) => {
  res.json({
    id: 'chatcmpl-123',
    object: 'chat.completion',
    created: Date.now(),
    model: req.body.model || 'gpt-4',
    choices: [{
      index: 0,
      message: { role: 'assistant', content: 'Mock response from test server' },
      finish_reason: 'stop'
    }],
    usage: { prompt_tokens: 10, completion_tokens: 20, total_tokens: 30 }
  });
});

// Embeddings
router.post('/embeddings', (req, res) => {
  const input = Array.isArray(req.body.input) ? req.body.input : [req.body.input];
  res.json({
    object: 'list',
    model: req.body.model || 'text-embedding-3-small',
    data: input.map((_, i) => ({
      index: i,
      embedding: Array(1536).fill(0.1)
    })),
    usage: { prompt_tokens: 10, total_tokens: 10 }
  });
});

// Models
router.get('/models', (req, res) => {
  res.json({
    object: 'list',
    data: [
      { id: 'gpt-4', object: 'model', created: 1687882411, owned_by: 'openai' },
      { id: 'gpt-3.5-turbo', object: 'model', created: 1677610602, owned_by: 'openai' }
    ]
  });
});

router.get('/models/:model', (req, res) => {
  res.json({
    id: req.params.model,
    object: 'model',
    created: 1687882411,
    owned_by: 'openai'
  });
});

// Moderations
router.post('/moderations', (req, res) => {
  res.json({
    id: 'modr-123',
    model: 'text-moderation-007',
    results: [{
      flagged: false,
      categories: {
        hate: false, 'hate/threatening': false, harassment: false,
        'harassment/threatening': false, 'self-harm': false, 'self-harm/intent': false,
        'self-harm/instructions': false, sexual: false, 'sexual/minors': false,
        violence: false, 'violence/graphic': false
      },
      category_scores: {
        hate: 0.001, 'hate/threatening': 0.001, harassment: 0.001,
        'harassment/threatening': 0.001, 'self-harm': 0.001, 'self-harm/intent': 0.001,
        'self-harm/instructions': 0.001, sexual: 0.001, 'sexual/minors': 0.001,
        violence: 0.001, 'violence/graphic': 0.001
      }
    }]
  });
});

// Images
router.post('/images/generations', (req, res) => {
  res.json({
    created: Date.now(),
    data: [{ url: 'https://example.com/image.png' }]
  });
});

// Audio
router.post('/audio/transcriptions', (req, res) => {
  res.json({ text: 'Mock transcription text' });
});

router.post('/audio/translations', (req, res) => {
  res.json({ text: 'Mock translation text' });
});

router.post('/audio/speech', (req, res) => {
  res.set('Content-Type', 'audio/mpeg');
  res.send(Buffer.from([0xFF, 0xFB, 0x90, 0x00]));
});

// Files
router.post('/files', (req, res) => {
  res.json({
    id: 'file-123',
    object: 'file',
    bytes: 1024,
    created_at: Date.now(),
    filename: 'test.jsonl',
    purpose: 'fine-tune'
  });
});

router.get('/files', (req, res) => {
  res.json({
    object: 'list',
    data: [{
      id: 'file-123',
      object: 'file',
      bytes: 1024,
      created_at: Date.now(),
      filename: 'test.jsonl',
      purpose: 'fine-tune'
    }]
  });
});

router.get('/files/:file_id', (req, res) => {
  res.json({
    id: req.params.file_id,
    object: 'file',
    bytes: 1024,
    created_at: Date.now(),
    filename: 'test.jsonl',
    purpose: 'fine-tune'
  });
});

router.get('/files/:file_id/content', (req, res) => {
  res.send('Mock file content');
});

router.delete('/files/:file_id', (req, res) => {
  res.json({ id: req.params.file_id, object: 'file', deleted: true });
});

// Batch
router.post('/batches', (req, res) => {
  res.json({
    id: 'batch-123',
    object: 'batch',
    endpoint: req.body.endpoint,
    input_file_id: req.body.input_file_id,
    completion_window: req.body.completion_window,
    status: 'validating',
    created_at: Date.now()
  });
});

router.get('/batches/:batch_id', (req, res) => {
  res.json({
    id: req.params.batch_id,
    object: 'batch',
    endpoint: '/v1/chat/completions',
    input_file_id: 'file-123',
    completion_window: '24h',
    status: 'completed',
    created_at: Date.now()
  });
});

router.post('/batches/:batch_id/cancel', (req, res) => {
  res.json({
    id: req.params.batch_id,
    object: 'batch',
    status: 'cancelling',
    created_at: Date.now()
  });
});

router.get('/batches', (req, res) => {
  res.json({
    object: 'list',
    data: [{
      id: 'batch-123',
      object: 'batch',
      status: 'completed',
      created_at: Date.now()
    }]
  });
});

// Fine-tuning
router.post('/fine_tuning/jobs', (req, res) => {
  res.json({
    id: 'ftjob-123',
    object: 'fine_tuning.job',
    model: req.body.model,
    created_at: Date.now(),
    status: 'validating_files',
    training_file: req.body.training_file
  });
});

router.get('/fine_tuning/jobs', (req, res) => {
  res.json({
    object: 'list',
    data: [{
      id: 'ftjob-123',
      object: 'fine_tuning.job',
      model: 'gpt-3.5-turbo',
      created_at: Date.now(),
      status: 'succeeded',
      training_file: 'file-123'
    }]
  });
});

router.get('/fine_tuning/jobs/:job_id', (req, res) => {
  res.json({
    id: req.params.job_id,
    object: 'fine_tuning.job',
    model: 'gpt-3.5-turbo',
    created_at: Date.now(),
    status: 'succeeded',
    training_file: 'file-123'
  });
});

router.post('/fine_tuning/jobs/:job_id/cancel', (req, res) => {
  res.json({
    id: req.params.job_id,
    object: 'fine_tuning.job',
    status: 'cancelled',
    created_at: Date.now()
  });
});

// Assistants
router.post('/assistants', (req, res) => {
  res.json({
    id: 'asst-123',
    object: 'assistant',
    created_at: Date.now(),
    model: req.body.model,
    name: req.body.name,
    instructions: req.body.instructions
  });
});

router.get('/assistants/:assistant_id', (req, res) => {
  res.json({
    id: req.params.assistant_id,
    object: 'assistant',
    created_at: Date.now(),
    model: 'gpt-4',
    name: 'Test Assistant',
    instructions: 'You are a helpful assistant'
  });
});

router.delete('/assistants/:assistant_id', (req, res) => {
  res.json({ id: req.params.assistant_id, object: 'assistant', deleted: true });
});

router.post('/threads', (req, res) => {
  res.json({ id: 'thread-123', object: 'thread', created_at: Date.now() });
});

router.post('/threads/:thread_id/messages', (req, res) => {
  res.json({
    id: 'msg-123',
    object: 'thread.message',
    created_at: Date.now(),
    thread_id: req.params.thread_id,
    role: req.body.role,
    content: [{ type: 'text', text: { value: req.body.content } }]
  });
});

router.post('/threads/:thread_id/runs', (req, res) => {
  res.json({
    id: 'run-123',
    object: 'thread.run',
    created_at: Date.now(),
    thread_id: req.params.thread_id,
    assistant_id: req.body.assistant_id,
    status: 'queued'
  });
});

router.get('/threads/:thread_id/runs/:run_id', (req, res) => {
  res.json({
    id: req.params.run_id,
    object: 'thread.run',
    created_at: Date.now(),
    thread_id: req.params.thread_id,
    assistant_id: 'asst-123',
    status: 'completed'
  });
});

// Videos
router.post('/videos/generations', (req, res) => {
  res.json({
    id: 'video-123',
    status: 'processing',
    created_at: Date.now()
  });
});

router.get('/videos/:video_id', (req, res) => {
  res.json({
    id: req.params.video_id,
    status: 'completed',
    url: 'https://example.com/video.mp4',
    created_at: Date.now()
  });
});

router.get('/videos', (req, res) => {
  res.json({
    data: [{
      id: 'video-123',
      status: 'completed',
      created_at: Date.now()
    }]
  });
});

// Completions (legacy)
router.post('/completions', (req, res) => {
  res.json({
    id: 'cmpl-123',
    object: 'text_completion',
    created: Date.now(),
    model: req.body.model,
    choices: [{
      text: 'Mock completion text',
      index: 0,
      finish_reason: 'stop'
    }],
    usage: { prompt_tokens: 10, completion_tokens: 20, total_tokens: 30 }
  });
});

module.exports = router;
