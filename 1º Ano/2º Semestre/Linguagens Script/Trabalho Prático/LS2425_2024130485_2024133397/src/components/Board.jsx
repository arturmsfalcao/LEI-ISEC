import React, { useEffect, useState } from "react";
import { Slot } from "./Slot";
import { getBotMove } from "../utils/bot";
import { Endgame } from "./Endgame";
import Timer from "./Timer";

export const Board = ({ players, gameMode }) => {
  const createEmptyBoard = () =>
    Array.from({ length: 6 }, () => Array(7).fill(""));

  const [board, setBoard] = useState(createEmptyBoard());
  const [currPlayer, setCurrPlayer] = useState("X");
  const [gameOver, setGameOver] = useState(false);
  const [draw, setDraw] = useState(false);
  const [hoverColumn, setHoverColumn] = useState(null);
  const [specialSpaces, setSpecialSpaces] = useState([]);

  // Timer state
  const [timer, setTimer] = useState(10);
  const initialTime = 10;

  useEffect(() => {
    if (!gameOver) {
      const first = Math.random() < 0.5 ? "X" : "O";
      setCurrPlayer(first);

      const newSpecialSpaces = new Set();
      while (newSpecialSpaces.size < 5) {
        const r = Math.floor(Math.random() * 6);
        const c = Math.floor(Math.random() * 7);
        newSpecialSpaces.add(`${r},${c}`);
      }
      setSpecialSpaces(Array.from(newSpecialSpaces));
    }
  }, [gameOver]);

  useEffect(() => {
    setTimer(initialTime);
  }, [currPlayer]);

  useEffect(() => {
    if (gameOver) return;

    if (timer === 0) {
      changeTurn();
      setTimer(initialTime);
      return;
    }

    const timerId = setTimeout(() => {
      setTimer(timer - 1);
    }, 1000);

    return () => clearTimeout(timerId);
  }, [timer, gameOver]);

  const changeTurn = () => {
    setCurrPlayer((prev) => (prev === "X" ? "O" : "X"));
  };

  const isBoardFull = () => {
    for (let row = 0; row < 6; row++) {
      for (let col = 0; col < 7; col++) {
        if (board[row][col] === "") {
          return false;
        }
      }
    }
    return true;
  };

  const checkWin = (row, col, ch) => {
    const directions = [
      [1, 0], [0, 1], [1, 1], [1, -1]
    ];

    for (let [dx, dy] of directions) {
      let count = 1;

      for (let dir of [-1, 1]) {
        let i = 1;
        while (true) {
          const r = row + i * dx * dir;
          const c = col + i * dy * dir;
          if (r < 0 || r >= 6 || c < 0 || c >= 7 || board[r][c] !== ch) break;
          count++;
          i++;
        }
      }

      if (count >= 4) return true;
    }

    return false;
  };

  const handleClick = (e) => {
    if (gameOver) return;
    if (gameMode === "bot" && currPlayer === "O") return;

    let column = parseInt(e.target.getAttribute("data-x"));
    if (isNaN(column) && e.target.parentElement) {
      column = parseInt(e.target.parentElement.getAttribute("data-x"));
    }
    if (isNaN(column)) return;

    let row = 5;
    while (row >= 0 && board[row][column] !== "") row--;

    if (row < 0) return;

    const newBoard = board.map((r) => [...r]);
    newBoard[row][column] = currPlayer;
    setBoard(newBoard);

    if (checkWin(row, column, currPlayer)) {
      setGameOver(true);
      return;
    }

    if (isBoardFull()) {
      setGameOver(true);
      setDraw(true);
      return;
    }

    const posKey = `${row},${column}`;
    if (specialSpaces.includes(posKey)) {

      return;
    }

    changeTurn();
  };

  const resetGame = () => {
    setBoard(createEmptyBoard());
    setGameOver(false);
    setDraw(false);
    setCurrPlayer("X"); // Reset current player to default or initial
    setTimer(initialTime); // Reset timer on game reset
  };

  useEffect(() => {
    if (gameOver) return;
    if (gameMode !== "bot") return;
    if (currPlayer !== "O") return;

    const botMoveTimeout = setTimeout(() => {
      const chosenCol = getBotMove(board, "O", "X");
      if (chosenCol === -1) return;

      let row = 5;
      while (row >= 0 && board[row][chosenCol] !== "") row--;

      if (row < 0) return;

      const newBoard = board.map((r) => [...r]);
      newBoard[row][chosenCol] = currPlayer;
      setBoard(newBoard);

      if (checkWin(row, chosenCol, currPlayer)) {
        setGameOver(true);
        return;
      }

      changeTurn();
    }, 1000);

    return () => clearTimeout(botMoveTimeout);
  }, [board, currPlayer, gameOver, gameMode]);

  return (
    <div>
      <Endgame gameOver={gameOver} winner={currPlayer} draw={draw} players={players} resetGame={resetGame} />

      {!gameOver ? (
        <>
          <div id="statusBox">
            <div id="playerName">
              {players[currPlayer]} A JOGAR
            </div>
            <Timer timer={timer} />
          </div>
        </>
      ) : null}

      <div id="hoverIndicator">
        {hoverColumn !== null && (
          <div
            className="arrow"
            style={{ left: `${hoverColumn * 95 + 57}px` }}
          >
            ↓
          </div>
        )}
      </div>
      
      <div id="board" onClick={handleClick}>
        {Array(7).fill(null).map((_, colIndex) => (
          <div
            key={colIndex}
            className="column"
            onMouseEnter={() => setHoverColumn(colIndex)}
            onMouseLeave={() => setHoverColumn(null)}
          >
            {Array(6).fill(null).map((_, rowIndex) => (
              <Slot key={`${rowIndex}-${colIndex}`} ch={board[rowIndex][colIndex]} y={rowIndex} x={colIndex} isSpecial={specialSpaces.includes(`${rowIndex},${colIndex}`)} />
            ))}
          </div>
        ))}  
      </div>
    </div>
  );
};
