/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 14:41:50 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/10 17:24:18 by arvoyer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ChessBot.hpp"
#include <fstream>
#include <iostream>

ChessBot::ChessBot()
{
	_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	
	std::ifstream	in("ChessBoard/ChessBoard.chess");
	if (in.is_open() == false)
	{
		_fen = "\0";
		return ;
	}
	
	_board = new std::string[45];
	
	std::string line;
	int			i;

	i = 0;
	while (in.eof() == false)
	{
		std::getline(in, line);
		_board[i] = line;
		_board[i] += "\n";
		i++;
	}
}

ChessBot::ChessBot(const ChessBot &copy)
{
	this->_fen = copy._fen;
	
	this->_board = new std::string[45];
	for (int i = 0; i < 45; i++)
		this->_board[i] = copy._board[i];
}

ChessBot &ChessBot::operator=(const ChessBot &other)
{
	if (this != &other)
	{
		this->_fen = other._fen;
		for (int i = 0; i < 45; i++)
			this->_board[i] = other._board[i];
	}
	return (*this);
}

ChessBot::~ChessBot()
{
	delete[] _board;
}

void	ChessBot::PrintBoard(std::string &response)
{
	for (int i = 0; i < 45; i++)
	{
		response += _board[i];
	}
	response += "\n";
}

void ChessBot::ClearCase(int i, int j)
{
	int iEnd = i + CASEWIDTH - 1;
	int jEnd = j + CASEHEIGHT - 1;

	char ClearChar = _board[j][i];
	
	for (; j < jEnd; j++)
	{
		for (; i < iEnd; i++)
		{
			_board[j][i] = ClearChar;
		}
		i -= CASEWIDTH - 1;
	}
}

void	ChessBot::PutPiece(std::string piece, int i, int j)
{
	std::ifstream	in(piece.c_str());
	if (in.is_open() == false)
	{
		_fen = "\0";
		return ;
	}

	i--;
	int starti = i;

	std::string line;

	while (in.eof() == false)
	{
		std::getline(in, line);
		for (int i2 = 0; line[i2]; i2++)
		{
			i++;
			if (!line[i2])
				break ;
			if (line[i2] == ' ' || line[i2] == '#')
				continue ;
			_board[j][i] = line[i2];
		}
		i = starti;
		j++;
	}
}

int		ChessBot::GetPosI(char pos)
{
	int	i = 0;
	
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
		default:
			return (piece_file); // error
	}
	return (piece_file);
}

int	ChessBot::GetFenPos(std::string pos)
{
	int	lineI = 8 - (pos[1] - '0');
	int	i = 0;

	while (lineI > 0)
	{
		if (_fen[i] == '/')
			lineI--;
		i++;
	}

	int lineJ = pos[0] - 'a';

	while (lineJ > 0)
	{
		if (_fen[i] >= '1' && _fen[i] <= '8')
			lineJ -= _fen[i] - '0';
		i++;
		lineJ--;
	}
	
	if (lineJ < 0)
		return (-1); // error
	return (i);
}

void	ChessBot::MovePiece(std::string movement)
{
	int	i = this->GetFenPos(movement);
	char piece = _fen[i];
	
	this->ClearCase(this->GetPosI(movement[0]), this->GetPosJ(movement[1]));
	std::string piece_file = this->GetPieceFile(piece);
	this->PutPiece(piece_file, this->GetPosI(movement[2]), this->GetPosJ(movement[3]));

	_fen[i] = '1';
	
	movement.erase(0, 2);
	i = this->GetFenPos(movement);
	// Finir le remplacement du fen avec la nouvelle pos de la piece et si la nouvelle pos est un chiffre autre que 1 change la string en consequence
}