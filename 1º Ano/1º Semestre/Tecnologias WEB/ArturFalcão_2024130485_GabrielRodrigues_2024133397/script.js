window.addEventListener('resize', () => {
  location.reload();
});

function atualizarLogo() {
  var logo = document.getElementById("logo");
  if (window.innerWidth <= 800) {
      logo.src = "img/logo2.png";
      logo.className = "logo1_img";
  } else {
      logo.src = "img/logo.png";
      logo.className = "logo_img";
  }
}

window.onload = atualizarLogo;
window.onresize = atualizarLogo;
const smallScreen = window.matchMedia("(max-width: 800px)");


function clickMenu() {
  
  if (smallScreen.matches) {
    const menu = document.querySelector('.menu');
    menu.classList.toggle('menu-open');
      if (nav.style.display == 'block'|| login.style.display == 'block' || navc.style.display == 'block') {
          nav.style.display = 'none'
          navc.style.display = 'none'
          navc1.style.display = 'none'
          login.style.display = 'none'
          document.body.style.overflow = 'auto';
      } else {
          nav.style.display = 'block'
          navc.style.display = 'block'
          navc1.style.display = 'block'
          login.style.display = 'block'
          document.body.style.overflow = 'hidden';
      }
  }
}

function clickMenu1() {
  if (smallScreen.matches) {
      if (menu1.style.display == 'block') {
          menu1.style.display = 'none'
      } else {
          menu1.style.display = 'block'
          menu2.style.display = 'none'
          menu3.style.display = 'none'
          menu4.style.display = 'none'
          menu5.style.display = 'none'
      }
  }
}   
function clickMenu2() {
  if (smallScreen.matches) {
      if (menu2.style.display == 'block') {
          menu2.style.display = 'none'
      } else {
          menu1.style.display = 'none'
          menu2.style.display = 'block'
          menu3.style.display = 'none'
          menu4.style.display = 'none'
          menu5.style.display = 'none'
      }
  }
}
function clickMenu3() {
  if (smallScreen.matches) {
      if (menu3.style.display == 'block') {
          menu3.style.display = 'none'
      } else {
          menu1.style.display = 'none'
          menu2.style.display = 'none'
          menu3.style.display = 'block'
          menu4.style.display = 'none'
          menu5.style.display = 'none'
      }
  }
}
function clickMenu4() {
  if (smallScreen.matches) {
      if (menu4.style.display == 'block') {
          menu4.style.display = 'none'
      } else {
          menu1.style.display = 'none'
          menu2.style.display = 'none'
          menu3.style.display = 'none'
          menu4.style.display = 'block'
          menu5.style.display = 'none'
      }
  }
}
function clickMenu5() {
  if (smallScreen.matches) {
      if (menu5.style.display == 'block') {
          menu5.style.display = 'none'
      } else {
          menu1.style.display = 'none'
          menu2.style.display = 'none'
          menu3.style.display = 'none'
          menu4.style.display = 'none'
          menu5.style.display = 'block'
      }
  }
}

document.addEventListener('DOMContentLoaded', function() {
  const menu = document.querySelector('.menu');
  if (menu) {
    menu.classList.add('menu-open');
  } else {
    console.error("Elemento '.menu' não encontrado.");
  }
});


document.addEventListener("DOMContentLoaded", function () {
    const radioButtons = document.querySelectorAll('input[name="search"]');
    const contentBoxes = document.querySelectorAll('.content-box');

    function updateVisibility() {
        const selectedValue = document.querySelector('input[name="search"]:checked').value;
        contentBoxes.forEach(box => {
            box.style.display = box.id === selectedValue ? 'block' : 'none';
        });
    }

    radioButtons.forEach(radio => {
        radio.addEventListener("change", updateVisibility);
    });

    updateVisibility();
});



let ultimaHora = null;

function verificarEstado() {
  const tipoVoo = document.getElementById("tipoVoo").value;
  const dataVoo = new Date(document.getElementById("dataVoo").value);
  const cidade = document.getElementById("cidade").value;
  const resultadoDiv = document.getElementById("resultado");

  if (isNaN(dataVoo)) {
    resultadoDiv.innerHTML = "<p>Please select a valid flight date.</p>";
    return;
  }

  let mensagem = "";
  const dia = dataVoo.getDate();
  const mes = dataVoo.getMonth() + 1;

  let horaVooInicial = cidade.toLowerCase() === "lisboa" ? "10:05" : "12:10";

  function horaVoo() {
    if (ultimaHora === null) {
      ultimaHora = horaVooInicial;
    } else {
      const [horas, minutos] = ultimaHora.split(":").map(Number);
      const novaHora = new Date();
      novaHora.setHours(horas, minutos, 0, 0);

      novaHora.setHours(novaHora.getHours() + 8);

      ultimaHora = `${novaHora.getHours().toString().padStart(2, '0')}:${novaHora.getMinutes().toString().padStart(2, '0')}`;
    }

    return ultimaHora;
  }

  const horaAjustada = horaVoo();

  if (tipoVoo === "ida") {
    mensagem = dia % 2 === 0 ? "On time" : "Delayed";
  } else if (tipoVoo === "regresso") {
    mensagem = mes % 2 === 0 ? "On time" : "Arrived";
  }

  resultadoDiv.innerHTML = `
    <p>Status: <strong> ${mensagem} </strong></p>
    <p>Type: <strong> ${tipoVoo === "ida" ? "Outbound" : "Return"} </strong></p>
    <p>Date: <strong> ${dataVoo.toLocaleDateString("pt-PT")} </strong></p>
    <p>Time: <strong> <span class="hora">${horaAjustada}</span> </strong></p>
    <p>City: <strong> ${cidade} </strong></p>
  `;
}