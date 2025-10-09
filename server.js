import express from "express";
import path from "path";
import { fileURLToPath } from "url";
import { createRequire } from "module";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const require = createRequire(import.meta.url);

const app = express();
const port = 3000;

// Load your compiled C++ addon
const calculator = require("./build/Release/calculator.node");

app.use(express.static(path.join(__dirname, "docs")));
app.use(express.json());

app.post("/evaluate", (req, res) => {
  const { expression } = req.body;
  try {
    const result = calculator.evaluate(expression);
    res.json({ result });
  } catch (e) {
    res.status(400).json({ error: e.message });
  }
});

// Start the server and keep the reference
const server = app.listen(port, () =>
  console.log(`Server running at http://localhost:${port}`)
);

// shutdown handlers
const shutdown = () => {
  console.log("\nShutting down server...");
  server.close(() => {
    console.log("Server closed cleanly.");
    process.exit(0);
  });
};

process.on("SIGINT", shutdown);   // Ctrl+C in terminal
process.on("SIGTERM", shutdown);  // kill command
process.once("SIGUSR2", () => {   // Nodemon restarts
  server.close(() => {
    console.log("Server closed for restart.");
    process.kill(process.pid, "SIGUSR2");
  });
});
