#include <stdafx.h> //pkh
#include <afxwin.h>         // MFC core and standard components

//#define _MANAGED  /* exclude compile problems */

#include "gpfuncs.h"
#include "errorlog.h"
#include "flwnode.h"
#include "gpwfuncs.h"
#include "neutralcommon.h"

#include "NETServer.h"
#include "NETServerU.h"

//========================================================================

#if SYSCAD10

//========================================================================

using namespace System;
using namespace System::IO;
using namespace System::Text;

using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Collections::Specialized;

using namespace System::Runtime::Remoting;
using namespace System::Runtime::Remoting::Channels;
using namespace System::Runtime::Remoting::Channels::Ipc;
using namespace System::Runtime::Remoting::Channels::Http;
using namespace System::Runtime::Remoting::Channels::Tcp;

using namespace System::Data;
using namespace System::Data::OleDb;
using namespace System::Drawing;
using namespace System::Diagnostics;

using namespace SysCAD::Protocol;
using namespace System::Runtime::Serialization::Formatters;
using namespace System::Runtime::Serialization::Formatters::Soap;

using namespace System::Runtime::InteropServices;//;::Marshal;

#pragma managed

//========================================================================
//
//
//
//========================================================================


CString ToCString(String ^ str)
  {
  // Pin memory so GC can't move it while native function is called
  pin_ptr<const wchar_t> wch = PtrToStringChars(str);
  //printf_s("%S\n", wch);

  // Conversion to char* :
  // Can just convert wchar_t* to char* using one of the 
  // conversion functions such as: 
  // WideCharToMultiByte()
  // wcstombs_s()
  // ... etc
  size_t convertedChars = 0;
  size_t  sizeInBytes = ((str->Length + 1) * 2);
  errno_t err = 0;
  char    *ch = (char *)malloc(sizeInBytes);

  err = wcstombs_s(&convertedChars, ch, sizeInBytes, wch, sizeInBytes);

  if (err != 0)
    LogError("NetServer", 0, "wcstombs_s  failed!\n");

  return CString(wch) ;
  }

//========================================================================
//
//
//
//========================================================================

ref class CNETServerThread
  {
  public:
    //EngineProtocol ^ engineProtocol;

    CNETServerThread()
      {
      // There could be sharing problems - when importing and editing simultaneously -  unlikely
      m_DoingExport=false;  
      };
    ~CNETServerThread()
      {
      };

    void Startup(String^ projectPath, String^ configPath, bool ImportScd9)
      {
      LogNote("CNETServerThread", 0, "Startup");

      BinaryServerFormatterSinkProvider^ serverProv = gcnew BinaryServerFormatterSinkProvider();
      serverProv->TypeFilterLevel = System::Runtime::Serialization::Formatters::TypeFilterLevel::Full;

      BinaryClientFormatterSinkProvider^ clientProv = gcnew BinaryClientFormatterSinkProvider();

      Hashtable^ tcpProps = gcnew Hashtable();
      tcpProps["port"] = "0";
      TcpChannel^ tcpChannel = gcnew TcpChannel(tcpProps, clientProv, serverProv);
      ChannelServices::RegisterChannel(tcpChannel, false);

      config = gcnew Config;
      engineProtocol = gcnew EngineProtocol;

      bool success = false;
      success = config->TestUrl(gcnew Uri("ipc://SysCAD.Service/Global"));

      if (!success) // couldn't connect to service, probably not running -- start one.
      {
      try
        {
        // Shell execute Service with parameters projectPath & configPath.
        Process ^proc = gcnew Process();
        proc->EnableRaisingEvents = false;

        String ^ progFiles = gcnew String(ProgFiles());

        String ^ stencilPath = gcnew String(BaseCfgFiles());
        stencilPath = stencilPath + "Stencils";

        proc->StartInfo->FileName = progFiles + "Service.exe";
        proc->StartInfo->Arguments = "\" " + projectPath + " \" \" " + configPath + " \" \" " + stencilPath + " \"";
        proc->Start();
        //proc->WaitForExit();
        }
      catch (Exception ^e)
        {
        // Handle this exception here, basically means the app didn't exist.
        }
      }

      success = false;
      int i=0;
      while ((!success)&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
        {
        // Basically need to wait until service is ready.
        Sleep(i*i*i); // Last wait will be 1sec.

        delete config;
        config = gcnew Config;
        // Connect to config data.
        success = config->TestUrl(gcnew Uri("ipc://SysCAD.Service/Global"));
        }
      if (success)
        {
        config->Syncxxx();


        success = false;
        i=0;
        while ((!success)&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
          {
          // Basically need to wait until service is ready.
          Sleep(i*i*i); // Last wait will be 1sec.

          delete engineProtocol;
          engineProtocol = gcnew EngineProtocol;

          // Connect to graphic data.
          success = engineProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + 
            Path::GetFileNameWithoutExtension(Path::GetDirectoryName(projectPath))));
          }
        if (success)
          {
          engineProtocol->Connect();

          /*
          This is where import goes
          */

          engineProtocol->GroupCreated += gcnew EngineProtocol::GroupCreatedHandler(this, &CNETServerThread::GroupCreated);
          engineProtocol->ItemCreated += gcnew EngineProtocol::ItemCreatedHandler(this, &CNETServerThread::ItemCreated);

          ////////////////////////////////
          ////////////////////////////////

          // Keith:
          // The lines above start the Service, connect to it, and sync the data.
          // You can see below the for each that will be needed to read through each item/link/thing.
          // (things are probably not required a the moment.)

          // Obviously these are empty until we 'bootstrap' the values from the current 9 graphics.
          // The Connect() tries to load the 10 graphics, and if it faile then the Count() of each 
          // dictionary will be 0.
          // A 'save' has to be done first to fill this, and then the 10 graphics can be used.

          for each (GraphicItem ^ item in engineProtocol->graphicItems->Values)
            {
            // 'Go To Definition' on GraphicItem doesn't go to the source but does show the
            // ObjectBrowser with all the available members.
            // e.g.
            // item->Angle
            // item->X

            SS_CreateItem(m_DoingExport, -1, -1, ToCString(item->Guid.ToString()), ToCString(item->Tag), ToCString(item->Path), 
              ToCString(item->Model->ToString()), ToCString(item->Shape->ToString()), 
              CRectangleF(item->BoundingRect.Left, item->BoundingRect.Right, item->BoundingRect.Top, item->BoundingRect.Bottom), 
              item->Angle, RGB(item->FillColor.R, item->FillColor.G, item->FillColor.B), 
              item->MirrorX, item->MirrorY);
            int yyy=0;
            //Chris
            }

          for each (GraphicLink ^ link in engineProtocol->graphicLinks->Values)
            {
            }

          for each (GraphicThing ^ thing in engineProtocol->graphicThings->Values)
            {
            }

          // GraphicItems is a dictionary of all the items.  The key is the guid, and the value is the item itself.
          // The dictionary class contains all the usual functions for extracting particular elements based on key, etc.
          // protocol->graphicItems->Keys returns a collection of keys.

          // For the 'save' we'll need to do a series of {Delete/Create/Modify}{Item/Link/Thing} to update the service.
          // Then call the Save function in protocol (and later config.)
          // This will cause the data service is holding to be serialized to disk (this might not work at first run.)

          // N.B. Strings are a bastard...  Converting from char* -> String is simply a case of declaring
          // 'String ^ a = gcnew String(charStarVar);'
          // But going the other way as far as I can tell can only be acheived by iterating through the entire String 
          // character by character and popping the null on the end manually.

          // N.B. There are two paths (here and in service.exe) that refer to the full path of the Service.exe
          // (search for /pkh/ and you'll find them.)
          // (I couldn't work out exactly how all the output-path stuff was working,
          // so I reverted back to a static path for now...)

          ///////////////////////////////
          ///////////////////////////////
          }
        }
      };


    void GroupCreated(Int64 eventId, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect)
      {
      int xxx=0;
      }

    void ItemCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ shape, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
      {
      SS_CreateItem(m_DoingExport, eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
        ToCString(model->ToString()), ToCString(shape->ToString()), //boundingRect, 
        CRectangleF(boundingRect.Left, boundingRect.Right, boundingRect.Top, boundingRect.Bottom), 
        angle, RGB(fillColor.R, fillColor.G, fillColor.B), 
        mirrorX, mirrorY);
      }

    void LinkCreated(Int64 eventId, Int64 requestID, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<PointF> controlPoints)
      {
      }

    void ThingCreated(Int64 eventId, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect, String^ xaml, Single angle, bool mirrorX, bool mirrorY)
      {
      }

    void ItemDeleted(Int64 eventId, Int64 requestID, Guid guid)
      {
      }

    void LinkDeleted(Int64 eventId, Int64 requestID, Guid guid)
      {
      }

    void ThingDeleted(Int64 eventId, Int64 requestID, Guid guid)
      {
      }

    void ItemModified(Int64 eventId, Int64 requestID, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, RectangleF boundingRect, Single angle, System::Drawing::Color fillColor, System::Drawing::Drawing2D::FillMode fillMode, bool mirrorX, bool mirrorY)
      {
      }

    void ItemPathModified(Int64 eventId, Int64 requestID, Guid guid, String^ path)
      {
      }

    void LinkModified(Int64 eventId, Int64 requestID, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<PointF> controlPoints)
      {
      }

    void ThingModified(Int64 eventId, Int64 requestID, Guid guid, String^ tag, String^ path, RectangleF boundingRect, String^ xaml, Single angle, bool mirrorX, bool mirrorY)
      {
      }

    void ThingPathModified(Int64 eventId, Int64 requestID, Guid guid, String^ path)
      {
      }


    void Shutdown()
      {
      delete config;//

      LogNote("CNETServerThread", 0, "Shutdown");
      };

    void Load(String^ filename)
      {
      engineProtocol->Load(filename);
      };

    void Save(String^ filename)
      {
      engineProtocol->Save(filename);
      };

    void Export(String ^ filename)
      {

      m_DoingExport=true;

      CGetExistingItems GI;

      int PrevPage=-1;

      static __int64 RqID=0;

      while (GI.GetOne())
        {
        CGrfTagInfo & I = GI.Item();

        dbgpln("XX %i %-20s %-20s %-20s", GI.Type(), I.m_sTag(), I.m_sSymbol(), I.m_sClass());
        System::Guid guid(gcnew String(GI.Guid()));
        guid=System::Guid::NewGuid();
        String ^ page =  gcnew String(GI.PageName());//"Pg1";

        // Simple Layout
        int NAcross=Max(1,int(Sqrt((double)GI.PageCount())+0.5));
        float XOffSet=(GI.PageNo()%NAcross)*310.0f*1.414f;
        float YOffSet=(GI.PageNo()/NAcross)*310.0f;

        if (PrevPage!=GI.PageNo())
          {
          PrevPage=GI.PageNo();

          String ^ path = "/" + filename + "/";
          engineProtocol->CreateGroup(RqID++, guid, gcnew String(GI.PageName()), gcnew String(path), RectangleF(XOffSet,YOffSet,297*1.4141f, 297));
          }

        switch (GI.Type())
          {
          case CGetExistingItems::eIsNode:
            {
            String ^ path = "/" + filename + "/" + page + "/";

            CString S(I.m_sSymbol());
            int iDollar=S.Find('$');
            if (iDollar>=0)
              S.Delete(0, iDollar+1);

            Model ^ model = gcnew Model(gcnew String(I.m_sClass()));
            Shape ^ shape = gcnew Shape(gcnew String(I.m_sClass()));

            engineProtocol->CreateItem(RqID++, guid, gcnew String(I.m_sTag()),
              path, model, shape,
              RectangleF(I.m_LoBnd.m_X+XOffSet, float((296.0f-I.m_HiBnd.m_Y+YOffSet)), 
              float(I.m_HiBnd.m_X-I.m_LoBnd.m_X), float(I.m_HiBnd.m_Y-I.m_LoBnd.m_Y)),
              0.0, Color(), Drawing2D::FillMode()  , false, false);
            break;
            }

          case CGetExistingItems::eIsLink:
            {
            break;
            }
          }
        }

      m_DoingExport=false;
      }

  protected:
    SysCAD::Protocol::Config ^ config;
    SysCAD::Protocol::EngineProtocol ^ engineProtocol;
    bool m_DoingExport;
  };


ref class CNETServerThreadGlbl
  {
  public:
    static CNETServerThread ^ gs_SrvrThread;// = gcnew CNETServerThread;
  };

//========================================================================
//
//
//
//========================================================================

CNETServer::CNETServer(void)
  {
  }

CNETServer::~CNETServer(void)
  {
  }

void CNETServer::Startup(char* projectPath, char* configPath, bool ImportScd9)
  {
  String^ projectPathString = gcnew String(projectPath);
  String^ configPathString = gcnew String(configPath);

  LogNote("CNETServer", 0, "Startup");

  CNETServerThreadGlbl::gs_SrvrThread = gcnew CNETServerThread;//.Startup("");
  CNETServerThreadGlbl::gs_SrvrThread->Startup(projectPathString, configPathString, ImportScd9);   

  //System::Threading::S
  };

void CNETServer::Shutdown()
  {
  CNETServerThreadGlbl::gs_SrvrThread->Shutdown();
  delete CNETServerThreadGlbl::gs_SrvrThread;

  LogNote("CNETServer", 0, "Shutdown");
  };

void CNETServer::Export(char* projectPath, char* configPath)
  {
  String^ projectPathString = gcnew String(projectPath);
  String^ configPathString = gcnew String(configPath);

  LogNote("CNETServer", 0, "Import");

  CNETServerThreadGlbl::gs_SrvrThread->Export(projectPathString);   

  //System::Threading::S
  };

void CNETServer::Load()
  {
  LogNote("CNETServer", 0, "Load");

  CNETServerThreadGlbl::gs_SrvrThread->Load("");
  };

void CNETServer::Save()
  {
  LogNote("CNETServer", 0, "Save");

  CNETServerThreadGlbl::gs_SrvrThread->Save("");
  };


//========================================================================

#endif

//========================================================================
