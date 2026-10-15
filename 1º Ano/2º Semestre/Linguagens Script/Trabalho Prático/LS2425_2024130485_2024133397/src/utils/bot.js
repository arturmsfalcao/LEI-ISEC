export function getBotMove(board, botChar, opponentChar) {
  const ROWS = 6;
  const COLS = 7;

  function getAvailableRow(board, col) {
    for (let r = ROWS - 1; r >= 0; r--) {
      if (board[r][col] === "") return r;
    }
    return -1;
  }

  const validColumns = [];
  for (let col = 0; col < COLS; col++) {
    if (getAvailableRow(board, col) !== -1) {
      validColumns.push(col);
    }
  }
  if (validColumns.length === 0) return -1;
  return validColumns[Math.floor(Math.random() * validColumns.length)];
}
