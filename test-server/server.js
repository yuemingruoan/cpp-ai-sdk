const express = require('express');
const bodyParser = require('body-parser');
const openaiRoutes = require('./routes/openai');
const anthropicRoutes = require('./routes/anthropic');
const googleRoutes = require('./routes/google');

const app = express();
const PORT = 8080;

app.use(bodyParser.json());
app.use(bodyParser.raw({ type: 'multipart/form-data', limit: '10mb' }));

app.use('/v1', openaiRoutes);
app.use('/v1', anthropicRoutes);
app.use('/v1beta', googleRoutes);

app.listen(PORT, () => {
  console.log(`Test server running on http://localhost:${PORT}`);
  console.log('OpenAI endpoints: http://localhost:8080/v1/*');
  console.log('Anthropic endpoints: http://localhost:8080/v1/*');
  console.log('Google endpoints: http://localhost:8080/v1beta/*');
});
