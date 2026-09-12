import React from "react";

export const Endgame = ({ gameOver, winner, draw, players, resetGame }) => {
  if (!gameOver) return null;

  return (
    <div className="modal-overlay">
      <div className="modal-content">
        <h1>Fim do jogo!</h1>
        {draw ? (
          <p>Empate! Não há mais espaços disponíveis para jogar.</p>
        ) : (
          <p>{players[winner]} venceu!</p>
        )}
        <button className="restart-button" onClick={resetGame}>
          Reiniciar Partida
        </button>
      </div>
    </div>
  );
};
