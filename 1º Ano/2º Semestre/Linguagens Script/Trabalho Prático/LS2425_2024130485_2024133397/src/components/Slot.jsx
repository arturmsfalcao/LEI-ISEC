import React from 'react';
import amarelo from '../assets/amarelo.png';
import vermelho from '../assets/vermelho.png';

export const Slot = ({ ch, y, x, isSpecial }) => {
    const slotStyle = isSpecial
        ? { boxShadow: 'inset 0 0 0 5px orange', borderRadius: '50%' }
        : { borderRadius: '50%' };

    return (
        <div className='slot' data-x={x} data-y={y} style={slotStyle}>
            {ch && (
                <img
                    src={ch === 'X' ? vermelho : amarelo}
                    width='100%'
                    height='100%'
                    style={{ borderRadius: '50%' }}
                />
            )}
        </div>
    );
};
