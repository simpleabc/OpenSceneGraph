#include <osg/Notify>

#include <osgDB/FileUtils>
#include <osgDB/FileNameUtils>

#include "OSGA_Archive.h"


class ReaderWriterOSGA : public osgDB::ReaderWriter
{
public:
    ReaderWriterOSGA() { }

    virtual const char* className() const { return "OpenSceneGraph Archive Reader/Writer"; }
    virtual bool acceptsExtension(const std::string& extension)
    {
        return osgDB::equalCaseInsensitive(extension,"osga");
    }

    virtual ReadResult openArchive(const std::string& file,ArchiveStatus status, unsigned int indexBlockSize = 4096, const Options* = NULL)
    {
        std::string ext = osgDB::getLowerCaseFileExtension(file);
        if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

        std::string fileName = osgDB::findDataFile( file );
        if (fileName.empty()) return ReadResult::FILE_NOT_FOUND;
    
        osg::ref_ptr<OSGA_Archive> archive = new OSGA_Archive;
        if (!archive->open(fileName, status, indexBlockSize))
        {
            return ReadResult(ReadResult::FILE_NOT_HANDLED);
        }

        return archive.get();
    }

    virtual ReadResult readObject(const std::string& /*fileName*/,const Options* =NULL) { return ReadResult(ReadResult::FILE_NOT_HANDLED); }
    virtual ReadResult readImage(const std::string& /*fileName*/,const Options* =NULL) { return ReadResult(ReadResult::FILE_NOT_HANDLED); }
    virtual ReadResult readHeightField(const std::string& /*fileName*/,const Options* =NULL) { return ReadResult(ReadResult::FILE_NOT_HANDLED); }

    virtual ReadResult readNode(const std::string& file,const Options* options)
    {
        ReadResult result = openArchive(file,osgDB::Archive::READ);
        
        if (!result.validArchive()) return result;


        osg::ref_ptr<ReaderWriter::Options> local_options = new ReaderWriter::Options;
        local_options->setDatabasePath(file);

        ReadResult result_2 = result.getArchive()->readNode(result.getArchive()->getMasterFileName(),local_options.get());
        

        // register the archive so that it is cached for future use.
        osgDB::Registry::instance()->addToArchiveCache(file, result.getArchive());


        return result_2;
    }

    virtual WriteResult writeObject(const osg::Object& /*obj*/,const std::string& /*fileName*/,const Options* =NULL) {return WriteResult(WriteResult::FILE_NOT_HANDLED); }
    virtual WriteResult writeImage(const osg::Image& /*image*/,const std::string& /*fileName*/,const Options* =NULL) {return WriteResult(WriteResult::FILE_NOT_HANDLED); }
    virtual WriteResult writeHeightField(const osg::HeightField& /*heightField*/,const std::string& /*fileName*/,const Options* =NULL) {return WriteResult(WriteResult::FILE_NOT_HANDLED); }
    virtual WriteResult writeNode(const osg::Node& /*node*/,const std::string& /*fileName*/,const Options* =NULL) { return WriteResult(WriteResult::FILE_NOT_HANDLED); }

protected:

    
};


// register with Registry to instantiate the above reader/writer.
osgDB::RegisterReaderWriterProxy<ReaderWriterOSGA> g_osgaReaderWriterProxy;
