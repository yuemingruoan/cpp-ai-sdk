const express = require('express');
const router = express.Router();

// Generate Content
router.post('/models/:model/generateContent', (req, res) => {
  res.json({
    candidates: [{
      content: {
        role: 'model',
        parts: [{ text: 'Mock response from Google test server' }]
      },
      finishReason: 'STOP'
    }],
    usageMetadata: {
      promptTokenCount: 10,
      candidatesTokenCount: 20,
      totalTokenCount: 30
    }
  });
});

// Stream Generate Content
router.post('/models/:model/streamGenerateContent', (req, res) => {
  res.json({
    candidates: [{
      content: {
        role: 'model',
        parts: [{ text: 'Mock streaming response' }]
      }
    }]
  });
});

// Embed Content
router.post('/models/:model/embedContent', (req, res) => {
  res.json({
    embedding: {
      values: Array(768).fill(0.1)
    }
  });
});

// Batch Embed Contents
router.post('/models/:model/batchEmbedContents', (req, res) => {
  const count = req.body.requests?.length || 1;
  res.json({
    embeddings: Array(count).fill({ values: Array(768).fill(0.1) })
  });
});

// Count Tokens
router.post('/models/:model/countTokens', (req, res) => {
  res.json({ totalTokens: 42 });
});

// Batch Generate Content
router.post('/models/:model/batchGenerateContent', (req, res) => {
  res.send('Mock batch generate response');
});

// File API
router.post('/files', (req, res) => {
  res.json({
    file: {
      name: 'files/file-123',
      displayName: 'test.pdf',
      mimeType: 'application/pdf',
      uri: 'https://example.com/file-123'
    }
  });
});

router.get('/files/:file_name', (req, res) => {
  res.json({
    name: req.params.file_name,
    displayName: 'test.pdf',
    mimeType: 'application/pdf',
    uri: 'https://example.com/file-123'
  });
});

router.delete('/files/:file_name', (req, res) => {
  res.status(204).send();
});

router.get('/files', (req, res) => {
  res.json({
    files: [{
      name: 'files/file-123',
      displayName: 'test.pdf',
      mimeType: 'application/pdf',
      uri: 'https://example.com/file-123'
    }]
  });
});

module.exports = router;
