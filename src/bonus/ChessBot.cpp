/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChessBot.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arvoyer <arvoyer@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/07 14:41:50 by arvoyer           #+#    #+#             */
/*   Updated: 2025/02/10 15:36:30 by arvoyer          ###   ########.fr       */
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

void ChessBot::ClearCase(std::string pos)
{
	int	i;
	int	j;

	this->GetPos(pos, i, j);

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

void	ChessBot::PutPiece(std::string piece, std::string pos)
{
	int	i;
	int	j;

	this->GetPos(pos, i, j);

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

void	ChessBot::GetPos(std::string pos, int &i, int &j) // changer en getposi et getposj dans le movepiece
{
	i = 0;
	while (i < pos[0] - 'a')
	{
		i++;
	}

	j = 0;
	while (j < pos[1] - '0')
	{
		j++;
	}
	j = 8 - j;

	i = SIDEBORDER + i * CASEWIDTH;
	j = TOPBORDER + j * CASEHEIGHT;
}

void	MovePiece(std::string movement)
{
	
}