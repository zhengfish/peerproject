//
// Datagram.h
//
// This file is part of PeerProject (peerproject.org) � 2008
// Portions Copyright Shareaza Development Team, 2002-2007.
//
// PeerProject is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 3
// of the License, or later version (at your option).
//
// PeerProject is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License 3.0
// along with PeerProject; if not, write to Free Software Foundation, Inc.
// 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA  (www.fsf.org)
//

#if !defined(AFX_DATAGRAM_H__72C119CA_1D67_4B22_A9DF_51398F17B756__INCLUDED_)
#define AFX_DATAGRAM_H__72C119CA_1D67_4B22_A9DF_51398F17B756__INCLUDED_

#pragma once

class CBuffer;
class CG2Packet;


class CDatagramIn
{
// Construction
public:
	CDatagramIn();
	virtual ~CDatagramIn();

// Attributes
public:
	CDatagramIn*	m_pNextHash;
	CDatagramIn**	m_pPrevHash;
	CDatagramIn*	m_pNextTime;
	CDatagramIn*	m_pPrevTime;
public:
	SOCKADDR_IN		m_pHost;
	BOOL			m_bCompressed;
	WORD			m_nSequence;
	BYTE			m_nCount;
	BYTE			m_nLeft;
	DWORD			m_tStarted;
public:
	CBuffer**		m_pBuffer;
	BOOL*			m_pLocked;
	BYTE			m_nBuffer;

// Operations
public:
	void		Create(SOCKADDR_IN* pHost, BYTE nFlags, WORD nSequence, BYTE nCount);
	BOOL		Add(BYTE nPart, LPCVOID pData, DWORD nLength);
	CG2Packet*	ToG2Packet();

};

#endif // !defined(AFX_DATAGRAM_H__72C119CA_1D67_4B22_A9DF_51398F17B756__INCLUDED_)