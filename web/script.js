function calculate() {
  const expr = document.getElementById("expr").value;
  try {
    // Quick evaluation (works with +, -, *, /)
    const result = eval(expr);
    document.getElementById("result").innerText = "= " + result;
  } catch {
    document.getElementById("result").innerText = "Error";
  }
}
