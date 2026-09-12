import React, { useState } from "react";
import { Board } from "./components/Board";
import "./App.css";
import Footer from "./components/Footer";

function App() {
  const [player1, setPlayer1] = useState("");
  const [player2, setPlayer2] = useState("");
  const [started, setStarted] = useState(false);
  const [gameMode, setGameMode] = useState("twoPlayers");

  const handleStart = (e) => {
    e.preventDefault();
    if (player1 && (player2 || gameMode === "bot")) {
      setStarted(true);
    }
  };

  return (
    <div className="App">
      <h3>4 EM LINHA - LINGUAGENS SCRIPT</h3>
      {!started ? (
        <form onSubmit={handleStart}>
          <h2>Digite os nomes dos jogadores</h2>
          <input
            type="text"
            placeholder="Jogador Vermelho"
            value={player1}
            onChange={(e) => setPlayer1(e.target.value)}
            required
          />
          {gameMode === "twoPlayers" && (
            <input
              type="text"
              placeholder="Jogador Amarelo"
              value={player2}
              onChange={(e) => setPlayer2(e.target.value)}
              required
            />
          )}
          <div>
            <label>
              <input
                type="radio"
                name="gameMode"
                value="twoPlayers"
                checked={gameMode === "twoPlayers"}
                onChange={() => setGameMode("twoPlayers")}
              />
              Dois Jogadores
            </label>
            <label style={{ marginLeft: "20px" }}>
              <input
                type="radio"
                name="gameMode"
                value="bot"
                checked={gameMode === "bot"}
                onChange={() => setGameMode("bot")}
              />
              Contra Computador
            </label>
          </div>
          <button type="submit">Iniciar Jogo</button>
      </form>
      ) : (
        <Board players={{ X: player1, O: gameMode === "bot" ? "Bot" : player2 }} gameMode={gameMode} />
      )}
      <Footer />
    </div>
  );
}

export default App;

