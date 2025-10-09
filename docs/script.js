const form = document.getElementById("calc-form");
const input = document.getElementById("expression");
const output = document.getElementById("result");

form.addEventListener("submit", async (e) => {
  e.preventDefault();
  const expr = input.value;

  const res = await fetch("/evaluate", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ expression: expr })
  });
  const data = await res.json();
  output.textContent = data.result ?? data.error;
});
