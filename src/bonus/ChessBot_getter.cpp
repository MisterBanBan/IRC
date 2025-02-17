/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot_getter.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/12 14:18:29 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/17 14:06:27 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChessBot.hpp"

int		ChessBot::GetPosI(char pos)
{
	int	i = 0;
	
	if (pos - 'a' < 0 || pos - 'a' > 7)
		return (-1);
	
	while (i < pos - 'a')
	{
		i++;
	}

	i = SIDEBORDER + i * CASEWIDTH;
	return (i);
}

int		ChessBot::GetPosJ(char pos)
{
	int	j = 0;

	if (pos - '0' < 1 || pos - '0' > 8)
		return (-1);

	while (j < pos - '0')
	{
		j++;
	}
	j = 8 - j;

	j = TOPBORDER + j * CASEHEIGHT;
	return (j);
}

std::string	ChessBot::GetPieceFile(char piece)
{
	std::string piece_file;
	
	switch (piece)
	{
		case 'r':
			piece_file = "ChessBoard/BlackRook.chess";
			break ;
		case 'n':
			piece_file = "ChessBoard/BlackKnight.chess";
			break ;
		case 'b':
			piece_file = "ChessBoard/BlackBishop.chess";
			break ;
		case 'k':
			piece_file = "ChessBoard/BlackKing.chess";
			break ;
		case 'q':
			piece_file = "ChessBoard/BlackQueen.chess";
			break ;
		case 'p':
			piece_file = "ChessBoard/BlackPawn.chess";
			break ;
		case 'R':
			piece_file = "ChessBoard/WhiteRook.chess";
			break ;
		case 'N':
			piece_file = "ChessBoard/WhiteKnight.chess";
			break ;
		case 'B':
			piece_file = "ChessBoard/WhiteBishop.chess";
			break ;
		case 'K':
			piece_file = "ChessBoard/WhiteKing.chess";
			break ;
		case 'Q':
			piece_file = "ChessBoard/WhiteQueen.chess";
			break ;
		case 'P':
			piece_file = "ChessBoard/WhitePawn.chess";
			break ;
	}
	return (piece_file);
}

int	ChessBot::GetFenPos(std::string pos)
{
	int	lineI = 8 - (pos[1] - '0');
	int	i = 0;

	while (lineI > 0)
	{
		if (_fen[0][i] == '/')
			lineI--;
		i++;
	}

	int lineJ = pos[0] - 'a';
	while (lineJ > 0)
	{
		if (_fen[0][i] >= '1' && _fen[0][i] <= '8')
			lineJ -= _fen[0][i] - '0';
		else
			lineJ--;
		i++;
	}
	
	if (lineJ < 0)
		return (i - 1);
	return (i);
}

void	ChessBot::MakeTableFromFen()
{
	int temp = 0;
	int	fen_i = 0;

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (!std::isalnum(_fen[0][fen_i]))
				fen_i++;
			if (std::isalpha(_fen[0][fen_i]))
				_table[i][j] = _fen[0][fen_i];
			else
			{
				if (temp <= 0)
					temp = _fen[0][fen_i] - '0';
				_table[i][j] = '0';
				temp--;
				if (temp > 0)
				continue ;
			}
			fen_i++;
		}
	}
}

void	ChessBot::MakeFenFromTable()
{
	int	fen_i = 0;

	_fen.insert(_fen.begin(), "xxxxxxxx/xxxxxxxx/xxxxxxxx/xxxxxxxx/xxxxxxxx/xxxxxxxx/xxxxxxxx/xxxxxxxx");
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (!std::isalnum(_fen[0][fen_i]))
				fen_i++;
			if (std::isalpha(_table[i][j]))
				_fen[0][fen_i] = _table[i][j];
			else
				_fen[0][fen_i] = '1';
			fen_i++;
		}	
	}
	for (int i = 0; _fen[0][i]; i++)
	{
		if (std::isdigit(_fen[0][i]) && i > 0 && std::isdigit(_fen[0][i - 1]))
		{
			_fen[0][i - 1] += (_fen[0][i] - '0');
			_fen[0].erase(i, 1);
			i--;
		}
	}
}
