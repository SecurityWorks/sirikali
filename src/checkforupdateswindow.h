/*
 *
 *  Copyright (c) 2020
 *  name : Francis Banyikwa
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CHECKFORUPDATESWINDOW_H
#define CHECKFORUPDATESWINDOW_H

#include <QDialog>
#include <QString>
#include <QCloseEvent>
#include <QMenu>
#include <QFile>
#include <QLocale>
#include <QJsonObject>
#include <QTableWidgetItem>

#include <utility>
#include <functional>

#include "utils/network_access_manager.hpp"
#include "utils/qprocess.hpp"

namespace Ui {
class checkforupdateswindow;
}

class checkforupdateswindow : public QDialog
{
	Q_OBJECT
public:
	using functions = std::pair< std::function< void() >,std::function< void() > > ;

	static checkforupdateswindow& instance( QWidget * parent,
						functions& function,
						utils::network::manager& nm )
	{
		return *( new checkforupdateswindow( parent,function,nm ) ) ;
	}
	checkforupdateswindow( QWidget * parent,functions&,utils::network::manager& ) ;
	struct args
	{
		template< typename Engine >
		args( const Engine& s )  :
			updatable( s.updatable() ),
			executableName( s.executableName() ),
			engineName( s.known() ? s.name() : "SiriKali" ),
			displayName( s.displayName().isEmpty() ? engineName : s.displayName() ),
			executableFullPath( s.executableFullPath() ),
			url( s.releaseURL() ),
			archiveName( s.onlineArchiveFileName() )
		{
		}
		bool updatable ;
		QString executableName ;
		QString engineName ;
		QString displayName ;
		QString installedVersion ;
		QString onLineVersion ;
		QString executableFullPath ;
		QString url ;
		QString archiveName ;
		QString error ;
		QJsonObject data ;
	} ;
	void add( const checkforupdateswindow::args& e ) ;
	void doneUpdating( bool ) ;
	void Show() ;
	~checkforupdateswindow() ;
signals:
	void closed() ;
private:
	class Ctx
	{
	public:
		Ctx( int row,const QString& path,const QString& tagName ) :
			m_row( row ),
			m_file( std::make_unique< QFile >( path ) ),
			m_tagName( tagName )
		{
		}
		void fileClose()
		{
			m_file->close() ;
		}
		bool fileOpen()
		{
			return m_file->open( QIODevice::WriteOnly ) ;
		}
		void fileWrite( const QByteArray& data )
		{
			m_file->write( data ) ;
		}
		QString filePath() const
		{
			return m_file->fileName() ;
		}
		Ctx move()
		{
			return std::move( *this ) ;
		}
		void fileRemove()
		{
			if( m_file->exists() ){

				m_file->remove() ;
			}
		}
		int row() const
		{
			return m_row ;
		}
		const QString& tagName() const
		{
			return m_tagName ;
		}
	private:
		int m_row ;
		std::unique_ptr< QFile > m_file ;
		QString m_tagName ;
	} ;
	void removeExtra( int ) ;
	void downloading( Ctx&,const utils::network::progress& ) ;
	void extracted( Ctx,const utils::qprocess::outPut& ) ;
	void currentItemChanged( QTableWidgetItem * ,QTableWidgetItem * ) ;
	void tableUpdate( int,const QString& ) ;
	const QString& archiveName( int ) ;
	QString tableText( int ) ;
	QString exePath( int ) ;
	QString exeName( int ) ;
	QString engineName( int ) ;
	QString engineDisplayName( int ) ;
	bool archivePath( const QString& ) ;
	void download( int,const QString&,const QString&,const QString& ) ;
	void extract( Ctx ) ;
	void noNeedToExtract( Ctx ) ;
	void updateComplete( const Ctx& ) ;
	void goToNext() ;
	void update( int ) ;
	void closeEvent( QCloseEvent * ) override ;
	void closeUI() ;
	QNetworkRequest networkRequest( const QString& url ) ;

	Ui::checkforupdateswindow * m_ui ;
	functions& m_functions ;

	class opts
	{
	public:
		opts( const checkforupdateswindow::args& e ) :
			m_data( e.data ),
			m_name( e.engineName ),
			m_executableName( e.executableName ),
			m_displayName( e.displayName ),
			m_archiveName( e.archiveName ),
			m_noError( e.error.isEmpty() ),
			m_updatable( e.updatable )
		{
		}
		const QJsonObject& data() const
		{
			return m_data ;
		}
		const QString& name() const
		{
			return m_name ;
		}
		const QString& executableName() const
		{
			return m_executableName ;
		}
		const QString& archiveName() const
		{
			return m_archiveName ;
		}
		const QString& displayName() const
		{
			return m_displayName ;
		}
		bool noError() const
		{
			return m_noError ;
		}
		bool updatable() const
		{
			return m_updatable ;
		}
	private:
		QJsonObject m_data ;
		QString m_name ;
		QString m_executableName ;
		QString m_displayName ;
		QString m_archiveName ;
		bool m_noError ;
		bool m_updatable ;
	} ;

	class locale
	{
	public:
		QString formattedDataSize( qint64 ) const ;
		static QString secondsToString( int s )
		{
			if( s < 3600 ){

				return QTime( 0,0,0,0 ).addSecs( s ).toString( "mm:ss" ) ;
			}else{
				return QTime( 0,0,0,0 ).addSecs( s ).toString( "hh:mm:ss" ) ;
			}
		}
	private:
		QLocale m_locale ;
	};

	std::vector< opts > m_opts ;
	utils::network::manager& m_network ;
	locale m_locale ;
	QString m_binPath ;
	int m_position = -1 ;
};

#endif // CHECKFORUPDATESWINDOW_H
