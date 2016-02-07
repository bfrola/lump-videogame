#pragma once

#include <list>

using namespace std;

/**
	\author Bernardino Frola

	\brief This class provide a simple managment of a finite set of string, for logging purposes.

	
 */
class UtilityLogger
{
public:
	UtilityLogger( unsigned int maxSize );
	~UtilityLogger(void);

	enum mLogLevels {
		LOG_DEBUG, 
		LOG_INFO
	};
	
	typedef string msgSenderType;
	typedef string msgTextType;	

	typedef pair<msgSenderType, msgTextType> logMsg;
	typedef pair<logMsg, mLogLevels> logItem;
	
	typedef list<logItem> msgListType;
	typedef list<logItem>::iterator msgListIteratorType;
	
	/**
		This method log a given message.
		\param <msgSender> Message sender.
		\param <msgText> The message to log.
		\param <level> The log level. One of UtilityLogger::mLogLevels.
	*/
	void log( msgSenderType msgSender, msgTextType msgText, mLogLevels level = LOG_INFO );
	
	/**
		\return True if the message senter is equal to the senter of the item.
	*/
	bool checkSender(logItem item);

	/**
		\return True if the message senter is greater or equal to the senter of the item.
	*/
	bool checkLevel(logItem item);

	/**
		\return The message enveloped into the item.
	*/
	string getMsgString(logItem item);
	
	/**
		Set the lof level.
	*/
	void setLevelString( string level );

	string getMsgListString( string separator );

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************
	
	msgListType getMsgList() { return mMsgList; }
	msgListIteratorType getIterator() { return mMsgList_it; }
	
	bool isModified() { return mModified; }
	void setModified( bool modified ) { mModified = modified; }

	void setSenderFilter( string senderFilter ) { mSenderFilter = senderFilter; }
	string getSenderFilter( void ) { return mSenderFilter; }

	// ************************************************************************
	// Campi
	// ************************************************************************
public:
	msgListType mMsgList;
	msgListIteratorType mMsgList_it;

protected:
	unsigned int mMaxSize;
	
	mLogLevels mLevel;

	// Flag utilizzato per sapere quando deve essere aggiornato visualizzazione
	bool mModified;
	
	// Filtro sul campo sender dei messaggi di logging
	string mSenderFilter;


};
 