#include <stdafx.h> //pkh
#include <afxwin.h>         // MFC core and standard components

//#define _MANAGED  /* exclude compile problems */

#include "gpfuncs.h"
#include "errorlog.h"
#include "flwnode.h"
#include "gpwfuncs.h"
#include "neutralcommon.h"

#include "SvcConn.h"
#include "SvcConnCLR.h"

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

using namespace SysCAD;
using namespace System::Windows::Forms;

using namespace System::Collections::ObjectModel;

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
    LogError("SvcConnCLR", 0, "wcstombs_s  failed!\n");

  return CString(wch) ;
  }

//========================================================================
//
//
//
//========================================================================

ref class CSvcConnectCLRThread
  {
  public:
    //EngineProtocol ^ engineProtocol;

    CSvcConnectCLRThread(CSvcConnect * pConn)
      {
      // There could be sharing problems - when importing and editing simultaneously -  unlikely
      m_pConn = pConn;
      m_Action = gcnew SysCAD::Protocol::Action();
      };
    ~CSvcConnectCLRThread()
      {
      for each (IChannel^ channel in ChannelServices::RegisteredChannels)
        ChannelServices::UnregisterChannel(channel);
      };

    void Startup()
      {

      LogNote("CSvcConnectCLRThread", 0, "Startup");

      BinaryServerFormatterSinkProvider^ serverProv = gcnew BinaryServerFormatterSinkProvider();
      serverProv->TypeFilterLevel = System::Runtime::Serialization::Formatters::TypeFilterLevel::Full;

      BinaryClientFormatterSinkProvider^ clientProv = gcnew BinaryClientFormatterSinkProvider();

      Hashtable^ tcpProps = gcnew Hashtable();
      tcpProps["port"] = "0";
      TcpChannel^ tcpChannel = gcnew TcpChannel(tcpProps, clientProv, serverProv);
      ChannelServices::RegisterChannel(tcpChannel, false);
    }

    bool ConfigSetup()
    {
      config = gcnew Config;
      clientProtocol = gcnew ClientProtocol;
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
          proc->StartInfo->Arguments = "\" " + stencilPath + " \"";
          proc->Start();
          //proc->WaitForExit();
          }
        catch (Exception^)
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

      return success;  
    };




    bool PrepareForUpgrade(String^ projectName, String^ projectPath)
      {
      config->Syncxxx();

      config->RemoteAddProjectAnyway(projectName, projectPath);

      bool clientSuccess = false;
      bool engineSuccess = false;
      int i=0;
      while ((!((clientSuccess)&&(engineSuccess)))&&(i++ < 20)) //_MUST_ find a better way to handle this! (but only temporary...)
        {
        // Basically need to wait until service is ready.
        Sleep(i*i*i); // Last wait will be 1sec.

        delete clientProtocol;
        delete engineProtocol;
        clientProtocol = gcnew ClientProtocol;
        engineProtocol = gcnew EngineProtocol;

        // Connect to graphic data.

        clientSuccess = clientProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Client/" + projectName));
        engineSuccess = engineProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + projectName));
        }
      if (clientSuccess&&engineSuccess)
        {
        clientProtocol->Connect("SysCAD9.1\nConnection: Client");
        engineProtocol->Connect("SysCAD9.1\nConnection: Engine");

        // This will allow the editor to create/delete in addition to modify after the project loads.
        __int64 requestId;
        engineProtocol->ChangeState(requestId, SysCAD::Protocol::EngineBaseProtocol::RunState::Edit);

        engineProtocol->PortInfoRequested += gcnew EngineProtocol::PortInfoRequestedHandler(this, &CSvcConnectCLRThread::PortInfoRequested);

        /*
        This is where import goes
        */

        clientProtocol->Changed += gcnew ClientProtocol::ChangedHandler(this, &CSvcConnectCLRThread::Changed);

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

        //for each (GraphicItem ^ item in clientProtocol->graphicItems->Values)
        //{
        // 'Go To Definition' on GraphicItem doesn't go to the source but does show the
        // ObjectBrowser with all the available members.
        // e.g.
        // item->Angle
        // item->X

        //CNM Removed must work out when to do this
        //m_pConn->OnCreateItem(-1, -1, ToCString(item->Guid.ToString()), ToCString(item->Tag), ToCString(item->Path), 
        //  ToCString(item->Model->ToString()), ToCString(item->Shape->ToString()), 
        //  CRectangleF(item->BoundingRect.Left, item->BoundingRect.Right, item->BoundingRect.Bottom, item->BoundingRect.Top), 
        //  item->Angle, RGB(item->FillColor.R, item->FillColor.G, item->FillColor.B), 
        //  item->MirrorX, item->MirrorY);
        //int yyy=0;
        //Chris
        //}

        //for each (GraphicLink ^ link in clientProtocol->graphicLinks->Values)
        //  {
        //  }

        //for each (GraphicThing ^ thing in clientProtocol->graphicThings->Values)
        //  {
        //  }



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

      if (clientProtocol == nullptr)
        return false;

      if ((clientProtocol->graphic->Groups == nullptr) || (clientProtocol->graphic->Nodes == nullptr) || 
        (clientProtocol->graphic->Links == nullptr) || (clientProtocol->graphic->Things == nullptr))
        return false;

      //return (!((clientProtocol->graphic->Groups->Count == 0)&&
      //  (clientProtocol->graphic->Nodes->Count == 0)&&
      //  (clientProtocol->graphic->Links->Count == 0)&&
      //  (clientProtocol->graphic->Things->Count == 0)));

      return true;
      };

    void Attach2Scd10()
      {
      //config = gcnew SysCAD::Protocol::Config();

      AttachProjectForm^ attachProjectForm = gcnew AttachProjectForm();
      if (attachProjectForm->ShowDialog() == DialogResult::OK)
        {
        attachProjectForm->ClientProtocol->Connect("SysCAD9.1\nConnection: Client");
        attachProjectForm->Config->Syncxxx();

        config = attachProjectForm->Config;
        clientProtocol = attachProjectForm->ClientProtocol;


        // Connect to graphic data.

        engineProtocol = gcnew EngineProtocol();
        engineProtocol->TestUrl(gcnew Uri("ipc://SysCAD.Service/Engine/" + clientProtocol->Name));
        engineProtocol->Connect("SysCAD9.1\nConnection: Engine");

        // This will allow the editor to create/delete in addition to modify after the project loads.
        __int64 requestId;
        engineProtocol->ChangeState(requestId, SysCAD::Protocol::EngineBaseProtocol::RunState::Edit);

        engineProtocol->PortInfoRequested += gcnew EngineProtocol::PortInfoRequestedHandler(this, &CSvcConnectCLRThread::PortInfoRequested);

        //clientProtocol->Changed += gcnew ClientProtocol::ChangedHandler(this, &CSvcConnectCLRThread::Changed);

        // now load 9
        m_pConn->DoOpenProject(ToCString(clientProtocol->Name), ToCString(clientProtocol->Path));

        // turf 9 grfs
        m_pConn->Remove9Graphics();
        
        // load grfs from 10  
        LoadGraphicsFromClient(clientProtocol->Path, clientProtocol->graphic);

        // check model list consistent
        CheckModels(clientProtocol->Path, clientProtocol->model);
    
        }
      };

    // ====================================================================

    void LoadGraphicsFromClient(String ^ path, SysCAD::Protocol::Graphic ^ Graphic)
      {
      m_pConn->dbgPrintLn("LoadGraphicsFromClient ---------------------------");

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicGroup ^> KGrp in clientProtocol->graphic->Groups)
        {
        SysCAD::Protocol::GraphicGroup ^ Grp = KGrp.Value;
        m_pConn->dbgPrintLn("Grf Grp: %s", ToCString(Grp->Tag->ToString()));
        m_pConn->GCBCreateGroup(ToCString(Grp->Guid.ToString()), ToCString(path->ToString()), ToCString(Grp->Tag->ToString()), CRectangleF(Grp->BoundingRect->Left, Grp->BoundingRect->Top, Grp->BoundingRect->Width, Grp->BoundingRect->Height));
        }

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicNode ^> KNd in clientProtocol->graphic->Nodes)
        {
        SysCAD::Protocol::GraphicNode ^ GNd = KNd.Value;
        m_pConn->dbgPrintLn("Grf Nd : %s", ToCString(GNd->Tag->ToString()));

        ModelNode ^MNd;
        if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
          {
          __int64 eventId=-1;
          __int64 requestId=-1;
          m_pConn->OnCreateNodeG(eventId, requestId, ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), 
            ToCString(MNd->NodeClass), ToCString(GNd->Shape), //boundingRect, 
            CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), 
            float(GNd->Angle), 
            CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible),
            RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), 
            GNd->MirrorX, GNd->MirrorY);
          }
        }

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicLink ^> KVPair in clientProtocol->graphic->Links)
        {
        SysCAD::Protocol::GraphicLink ^ GLnk = KVPair.Value;
        m_pConn->dbgPrintLn("Grf Lnk: %s", ToCString(GLnk->Tag->ToString()));

        CPointFList Pts;
        for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
          Pts.AddTail(CPointF(Pt->X, Pt->Y));
        __int64 eventId=-1;
        __int64 requestId=-1;
        m_pConn->OnCreateLinkG(eventId, requestId, ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", 
          CSvcGuidPair(ToCString(GLnk->Origin.ToString()), ToCString(GLnk->Destination.ToString())),
          Pts, 
          CSvcTagBlk(CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height),
          float(GLnk->TagAngle), GLnk->TagVisible));

        }
      }

    // ====================================================================

    void CheckModels(String ^ path, SysCAD::Protocol::Model ^ Model)
      {
      m_pConn->dbgPrintLn("CheckModels -------------------------------------- STILL TO DO");

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::ModelNode ^> KNd in clientProtocol->model->Nodes)
        {
        SysCAD::Protocol::ModelNode ^ Nd = KNd.Value;
        m_pConn->dbgPrintLn("Mdl Nd : %s", ToCString(Nd->Tag->ToString()));
        }

      for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::ModelLink ^> KVPair in clientProtocol->model->Links)
        {
        SysCAD::Protocol::ModelLink ^ Lnk = KVPair.Value;
        m_pConn->dbgPrintLn("Mdl Lnk: %s", ToCString(Lnk->Tag->ToString()));
        }
      }

    // ====================================================================
    //
    // ====================================================================

    void AddCreateGroup(__int64 & requestId, LPCSTR GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
      {
      GraphicGroup ^ Grp = gcnew GraphicGroup(Guid(gcnew String(GrpGuid)), gcnew String(Tag));
      Grp->Path = gcnew String(Path);
      Grp->BoundingRect = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());

      m_Action->Create->Add(Grp);
      }

    // ====================================================================
    //
    // ====================================================================

    void AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
      LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
      double Angle, const CSvcTagBlk & TagBlk, COLORREF FillColor, 
      bool MirrorX, bool MirrorY)
      {
      //PKH Comment- Is this correct
      GraphicNode ^ GNd = gcnew GraphicNode(Guid(gcnew String(GraphicGuid)), gcnew String(Tag), gcnew String(Path), Guid(gcnew String(ModelGuid)), 
        gcnew String(Symbol), gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height()),
        0.0, gcnew SysCAD::Protocol::Rectangle(TagBlk.m_Area.Left(), TagBlk.m_Area.Bottom(), TagBlk.m_Area.Width(), TagBlk.m_Area.Height()), TagBlk.m_Angle, TagBlk.m_Visible, Color::Empty, 
        Drawing2D::FillMode::Alternate, MirrorX, MirrorY);

      ModelNode ^ MNd = gcnew ModelNode(Guid(gcnew String(ModelGuid)), gcnew String(Tag), gcnew String(ClassId));

      m_Action->Create->Add(MNd);
      m_Action->Create->Add(GNd);

      }

    // ====================================================================
    //
    // ====================================================================

    void AddDeleteNode(__int64 & requestId, LPCSTR GraphicGuid)
      {
      SysCAD::Protocol::Action ^action = gcnew SysCAD::Protocol::Action();

      action->Delete->Add(Guid(gcnew String(GraphicGuid)));

      clientProtocol->Change(requestId, action);
      };

    //void ItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //  {
    //  m_pConn->OnDeleteItem(eventId, requestId, ToCString(guid.ToString()));
    //  }

    // ====================================================================
    //
    // ====================================================================

    void AddModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta)
      {
      Guid graphicGuid(gcnew String(GraphicGuid));
      GraphicNode ^ GNd;
      if (clientProtocol->graphic->Nodes->TryGetValue(graphicGuid, GNd))
        {

        GraphicNode ^ newGNd = GNd->Clone();
        newGNd->BoundingRect->X += (float)Delta.X;
        newGNd->BoundingRect->Y += (float)Delta.Y;
        newGNd->TagArea->X += (float)Delta.X;
        newGNd->TagArea->Y += (float)Delta.Y;

        SysCAD::Protocol::Action ^action = gcnew SysCAD::Protocol::Action();

        action->Modify->Add(newGNd);

        clientProtocol->Change(requestId, action);
        }
      };

    void AddModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, const CSvcTagBlk & TagBlk)
      {
      Guid graphicGuid(gcnew String(GraphicGuid));

      GraphicNode ^ GNd;
      if (clientProtocol->graphic->Nodes->TryGetValue(graphicGuid, GNd))
        {
        GraphicNode ^ newGNd = GNd->Clone();

        float OldW = newGNd->TagArea->Width;
        float NewW = OldW * TagBlk.m_Area.Height()/newGNd->TagArea->Height;
        newGNd->TagArea->X += (float)Delta.X-0.5*(NewW-OldW);
        newGNd->TagArea->Y += (float)Delta.Y;
        newGNd->TagArea->Width = NewW;
        newGNd->TagArea->Height = TagBlk.m_Area.Height();
        newGNd->TagAngle = TagBlk.m_Angle;
        newGNd->TagVisible = TagBlk.m_Visible;

        SysCAD::Protocol::Action ^action = gcnew SysCAD::Protocol::Action();

        action->Modify->Add(newGNd);

        clientProtocol->Change(requestId, action);
        return;
        }

      GraphicLink ^ LNd;
      if (clientProtocol->graphic->Links->TryGetValue(graphicGuid, LNd))
        {
        GraphicLink ^ newLNd = LNd->Clone();

        float OldW = newLNd->TagArea->Width;
        float NewW = OldW * TagBlk.m_Area.Height()/newLNd->TagArea->Height;
        newLNd->TagArea->X += (float)Delta.X-0.5*(NewW-OldW);
        newLNd->TagArea->Y += (float)Delta.Y;
        newLNd->TagArea->Width = NewW;
        newLNd->TagArea->Height = TagBlk.m_Area.Height();
        newLNd->TagAngle = TagBlk.m_Angle;
        newLNd->TagVisible = TagBlk.m_Visible;

        SysCAD::Protocol::Action ^action = gcnew SysCAD::Protocol::Action();

        action->Modify->Add(newLNd);

        clientProtocol->Change(requestId, action);
        return;
        }
      };

    //void DoModifyItem(__int64 & requestId, LPCSTR ItemGuid, LPCSTR Tag, LPCSTR Path, 
    //  LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect, 
    //  float Angle, const CRectangleF & tagArea, COLORREF FillColor, 
    //  bool MirrorX, bool MirrorY)
    //  {
    //  SysCAD::Protocol::Rectangle^ BR(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
    //  SysCAD::Protocol::Rectangle^ TA(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());
    //  
    //  //clientProtocol->ModifyItem(requestId, guid, gcnew String(Tag), gcnew String(Path), gcnew String(ClassId), gcnew String(Symbol), SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ tagArea, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    //  clientProtocol->ModifyItem(requestId, Guid(gcnew String(ItemGuid)), gcnew String(Tag), gcnew String(Path), 
    //    gcnew String(ClassId), gcnew String(Symbol), BR, Angle, TA,
    //    Color::Empty, Drawing2D::FillMode::Alternate, MirrorX, MirrorY);
    //    
    //  };

    //void ItemModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, Model^ model, Shape^ stencil, SysCAD::Protocol::Rectangle^ boundingRect, double angle, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle, System::Drawing::Color fillColor, bool mirrorX, bool mirrorY)
    //  {
    //  m_pConn->OnModifyItem(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(path), 
    //    ToCString(model->ToString()), ToCString(stencil->ToString()), //boundingRect, 
    //    CRectangleF(boundingRect->Left, boundingRect->Top, boundingRect->Width, boundingRect->Height), 
    //    angle, 
    //    CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height), 
    //tagAngle,
    //    RGB(fillColor.R, fillColor.G, fillColor.B), 
    //    mirrorX, mirrorY);
    //  }

    //void ItemPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
    //  {
    //  }                                

    // ====================================================================
    //
    // ====================================================================

    void AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, 
      LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, 
      LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
      LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
      LPCSTR OriginPort, LPCSTR DestinationPort, 
      CPointFList & ControlPoints, const CSvcTagBlk & TagBlk)
      {
      List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      POSITION Pos=ControlPoints.GetHeadPosition();
      while (Pos)
        {
        CPointF & Pt=ControlPoints.GetNext(Pos);
        Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
        }

      //SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());

      //m_Creates->As
      //clientProtocol->CreateLink(requestId, gcnew Guid(gcnew String(ModelGuid)), gcnew Guid(gcnew String(GraphicGuid)), gcnew String(Tag), /*gcnew String(Path), */ gcnew String(ClassId), Guid(gcnew String(OriginGuid)), Guid(gcnew String(DestinationGuid)), 
      //  gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, tagAngle);
      //LinkGuid = guid.ToString();

      try
        {

        GraphicLink ^ GLnk = gcnew GraphicLink(Guid(gcnew String(GraphicGuid)), Guid(gcnew String(ModelGuid)), gcnew String(Tag), 
          Guid(gcnew String(OriginGrfGuid)), 0, Guid(gcnew String(DestinationGrfGuid)), 0,  
          Pts, gcnew SysCAD::Protocol::Rectangle(TagBlk.m_Area.Left(), TagBlk.m_Area.Bottom(), TagBlk.m_Area.Width(), TagBlk.m_Area.Height()), TagBlk.m_Angle, TagBlk.m_Visible);

        //  gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, tagAngle);
        //LinkGuid = guid.ToString();



        //GLnk->Tag = gcnew String(Tag);
        //GLnk->Path = gcnew String(Path);
        //GLnk->LinkClass = gcnew String(ClassId);
        //GLnk->Shape = gcnew String(Symbol);
        //GLnk->BoundingRect = gcnew SysCAD::Protocol::Rectangle(boundingRect.Left(), boundingRect.Bottom(), boundingRect.Width(), boundingRect.Height());
        //GLnk->TextArea = gcnew SysCAD::Protocol::Rectangle(tagArea.Left(), tagArea.Bottom(), tagArea.Width(), tagArea.Height());
        //GLnk->TextAngle = Angle;
        //GLnk->FillColor = Color::Empty;                  // FillColor ?????
        //GLnk->FillMode = Drawing2D::FillMode::Alternate; // ???
        //GLnk->MirrorX = MirrorX;
        //GLnk->MirrorY = MirrorY;

        ModelLink ^ MLnk = gcnew ModelLink(Guid(gcnew String(ModelGuid)), gcnew String(Tag), gcnew String(ClassId),
          Guid(gcnew String(OriginMdlGuid)), Guid(gcnew String(DestinationMdlGuid)), gcnew String(OriginPort), gcnew String(DestinationPort));
        //MLnk->Tag = gcnew String(Tag);
        //MLnk->LinkClass = gcnew String(ClassId);

        //List<Item^>  ^Create = gcnew List<Item^>; 
        //List<Item^>  ^Modify;// = gcnew List<Item^>;
        //List<Guid>   ^Delete;// = gcnew List<Guid>;  

        m_Action->Create->Add(MLnk);
        m_Action->Create->Add(GLnk);
        }
      catch (Exception^)
        {
        }
      };

    //void LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String ^ tag, String ^ classId, Guid origin, Guid destination, 
    //    String ^ originPort, Int16 originPortID, String ^destinationPort, Int16 destinationPortID, List<SysCAD::Protocol::Point^> ^ controlPoints, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle)
    //    {
    //    CPointFList Pts;

    //    for each (SysCAD::Protocol::Point^ Pt in controlPoints)
    //      {
    //      Pts.AddTail(CPointF(Pt->X, Pt->Y));
    //      }

    //    m_pConn->OnCreateLink(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(classId), ToCString(origin.ToString()), ToCString(destination.ToString()), ToCString(originPort), ToCString(destinationPort), 
    //		Pts, CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height), tagAngle);
    //    }

    // ====================================================================
    //
    // ====================================================================

    void AddDeleteLink(__int64 & requestId, LPCSTR GraphicGuid)
      {
      SysCAD::Protocol::Action ^action = gcnew SysCAD::Protocol::Action();

      Guid ^ guid = gcnew Guid(gcnew String(GraphicGuid));
      SysCAD::Protocol::GraphicLink ^GLnk;

      if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
        {
        //ModelLink ^MLnk = GLnk->ModelGuid;

        action->Delete->Add(GLnk->ModelGuid);
        //action->Delete->Add(GLnk->Guid);

        m_pConn->dbgPrintLn("Init  Delete Lnk Mdl  : %s", ToCString(GLnk->ModelGuid.ToString()));
        //m_pConn->dbgPrintLn("Delete Lnk Grf  : %s", ToCString(GLnk->Guid->ToString()));

        for each (System::Collections::Generic::KeyValuePair<System::Guid, SysCAD::Protocol::GraphicLink ^> KVPair in clientProtocol->graphic->Links)
          {
          SysCAD::Protocol::GraphicLink ^ GLnk1 = KVPair.Value;
          if (GLnk->ModelGuid == GLnk1->ModelGuid)
            {
            action->Delete->Add(GLnk1->Guid);
            m_pConn->dbgPrintLn("Check Delete Lnk Grf  : %s", ToCString(GLnk1->Guid.ToString()));
            }
          } 
        }
      //action->Delete->Add(Guid(gcnew String(GraphicGuid)));
      //action->Delete->Add(Guid(gcnew String(ModelGuid)));

      clientProtocol->Change(requestId, action);
      //clientProtocol->DeleteLink(requestId, Guid(gcnew String(GraphicGuid)));
      };

    //void LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //  {
    //  m_pConn->OnDeleteLink(eventId, requestId, ToCString(guid.ToString()));
    //  }

    // ====================================================================
    //
    // ====================================================================


    void AddModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, LPCSTR ClassId, const CSvcGuidPair & Guids, LPCSTR OriginPort, LPCSTR DestinationPort, 
      CPointFList & ControlPoints, const CSvcTagBlk & TagBlk)
      {
      List<SysCAD::Protocol::Point^> ^ Pts = gcnew List<SysCAD::Protocol::Point^>;
      POSITION Pos=ControlPoints.GetHeadPosition();
      while (Pos)
        {
        CPointF & Pt=ControlPoints.GetNext(Pos);
        Pts->Add(gcnew SysCAD::Protocol::Point(Pt.X(), Pt.Y()));
        }
      SysCAD::Protocol::Rectangle^ TA = gcnew SysCAD::Protocol::Rectangle(TagBlk.m_Area.Left(), TagBlk.m_Area.Bottom(), TagBlk.m_Area.Width(), TagBlk.m_Area.Height());

      //clientProtocol->ModifyLink(requestId, Guid(gcnew String(LinkGuid)), gcnew String(Tag), /*LPCSTR Path,*/ gcnew String(ClassId), Guid(gcnew String(OriginGuid)), Guid(gcnew String(DestinationGuid)), 
      //  gcnew String(OriginPort), -1, gcnew String(DestinationPort), -1, Pts, TA, tagAngle);
      };

    //void LinkModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<SysCAD::Protocol::Point^> ^ controlPoints, SysCAD::Protocol::Rectangle^ tagArea, double tagAngle)
    //  {
    //  CPointFList Pts;
    //  for each (SysCAD::Protocol::Point ^ Pt in controlPoints)
    //    {
    //    Pts.AddTail(CPointF(Pt->X, Pt->Y));
    //    }

    //  // Ensure we don't have nulls so the ToString() call succeeds and passes on an empty string in the 
    //  // case where there is no destination or origin port.
    //  if (String::IsNullOrEmpty(originPort))      originPort = String::Empty;
    //  if (String::IsNullOrEmpty(destinationPort)) destinationPort = String::Empty;

    //  m_pConn->OnModifyLink(eventId, requestId, ToCString(guid.ToString()), ToCString(tag), ToCString(classId), ToCString(origin.ToString()), ToCString(destination.ToString()), ToCString(originPort), ToCString(destinationPort), 
    //Pts, CRectangleF(tagArea->Left, tagArea->Top, tagArea->Width, tagArea->Height), 
    //tagAngle);
    //  }


    // ====================================================================
    //
    // ====================================================================

    //void LinkCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ classId, Guid origin, Guid destination, String^ originPort, String^ destinationPort, List<PointF> controlPoints)
    //  {
    //  }


    void ThingCreated(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect, String^ xaml, double angle, bool mirrorX, bool mirrorY)
      {
      }

    //void LinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    //  {
    //  }

    void ThingDeleted(Int64 eventId, Int64 requestId, Guid guid)
      {
      }

    void ThingModified(Int64 eventId, Int64 requestId, Guid guid, String^ tag, String^ path, SysCAD::Protocol::Rectangle^ boundingRect, String^ xaml, double angle, bool mirrorX, bool mirrorY)
      {
      }

    void ThingPathModified(Int64 eventId, Int64 requestId, Guid guid, String^ path)
      {
      }



    //{

    //Int64 requestId;
    //Guid guid;
    //String ^ tag;
    //String ^ path;
    //String ^ model;
    //String ^ shape;
    //SysCAD::Protocol::Rectangle^ boundingRect;
    //Single angle;
    //System::Drawing::Color fillColor;
    //System::Drawing::Drawing2D::FillMode fillMode;
    //bool mirrorX;
    //bool mirrorY;

    //engineProtocol->ModifyItem(requestId, guid, tag, path, model, shape, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    //}

    bool ProcessChangeLists(Int64 requestId)
      {
      bool Ret=clientProtocol->Change(requestId, m_Action);
      m_Action->Clear();
      return Ret;
      }


    void PortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String^ tag)
      {
      PortInfo^ portInfo = gcnew PortInfo(PortStatusEnum::Available);
      engineProtocol->RequestPortInfo(requestId, guid, tag, portInfo);
      }

    void Changed(Int64 eventId, Int64 requestId, SysCAD::Protocol::Actioned ^actioned)
      {
      if (actioned->Created != nullptr)
        ProcessCreatedList(eventId, requestId, actioned->Created);

      if (actioned->Modified != nullptr)
        ProcessModifiedList(eventId, requestId, actioned->Modified);

      if (actioned->Deleted != nullptr)
        ProcessDeletedList(eventId, requestId, actioned->Deleted);
      }

    void ProcessCreatedList(Int64 eventId, Int64 requestId, Collection<Guid> ^ created)
      {
      for each (Guid ^ guid in created )
        {

        GraphicGroup ^GGrp;
        ModelNode ^ MNd;
        GraphicNode ^GNd;
        ModelLink ^ MLnk;
        GraphicLink ^GLnk;

        if (clientProtocol->graphic->Groups->TryGetValue(*guid, GGrp))
          {
          m_pConn->OnCreateGroup(eventId, requestId, ToCString(GGrp->Guid.ToString()), ToCString(GGrp->Tag), ToCString(GGrp->Path), CRectangleF(GGrp->BoundingRect->Left, GGrp->BoundingRect->Top, GGrp->BoundingRect->Width, GGrp->BoundingRect->Height)); 
          }

        else if (clientProtocol->model->Nodes->TryGetValue(*guid, MNd))
          {
          m_pConn->OnCreateNodeM(eventId, requestId, ToCString(MNd->Guid.ToString()), ToCString(MNd->Tag),  
            ToCString(MNd->NodeClass));
          }

        else if (clientProtocol->graphic->Nodes->TryGetValue(*guid, GNd))
          {
          ModelNode ^MNd;
          if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
            {
            m_pConn->OnCreateNodeG(eventId, requestId, ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), 
              ToCString(MNd->NodeClass), ToCString(GNd->Shape), //boundingRect, 
              CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), 
              float(GNd->Angle), 
              CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible),
              RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), 
              GNd->MirrorX, GNd->MirrorY);
            }
          }

        else if (clientProtocol->model->Links->TryGetValue(*guid, MLnk))
          {
          m_pConn->OnCreateLinkM(eventId, requestId, ToCString(MLnk->Guid.ToString()), ToCString(MLnk->Tag), ToCString(MLnk->LinkClass), 
            CSvcGuidPair(ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString())), ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort));
          }

        else if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {

          CPointFList Pts;
          for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
            {
            Pts.AddTail(CPointF(Pt->X, Pt->Y));
            }
          m_pConn->OnCreateLinkG(eventId, requestId, ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", 
            CSvcGuidPair(ToCString(GLnk->Origin.ToString()), ToCString(GLnk->Destination.ToString())),
            Pts, 
            CSvcTagBlk(CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height), float(GLnk->TagAngle), GLnk->TagVisible));
          }
        }
      }

    void ProcessModifiedList(Int64 eventId, Int64 requestId, Collection<Guid> ^ modified)
      {
      for each (Guid ^ guid in modified)
        {
        ModelNode ^ MNd;
        if (engineProtocol->model->Nodes->TryGetValue(*guid, MNd))
          {
          m_pConn->OnModifyNodeM(eventId, requestId, ToCString(MNd->Guid.ToString()), ToCString(MNd->Tag), ToCString(MNd->NodeClass));
          }

        GraphicNode ^GNd;
        if (clientProtocol->graphic->Nodes->TryGetValue(*guid, GNd))
          {
          ModelNode ^MNd;
          if (clientProtocol->model->Nodes->TryGetValue(GNd->ModelGuid, MNd))
            {
            m_pConn->OnModifyNodeG(eventId, requestId, ToCString(GNd->Guid.ToString()), ToCString(GNd->Tag), ToCString(GNd->Path), 
              ToCString(MNd->NodeClass), ToCString(GNd->Shape), //boundingRect, 
              CRectangleF(GNd->BoundingRect->Left, GNd->BoundingRect->Top, GNd->BoundingRect->Width, GNd->BoundingRect->Height), 
              float(GNd->Angle), 
              CSvcTagBlk(CRectangleF(GNd->TagArea->Left, GNd->TagArea->Top, GNd->TagArea->Width, GNd->TagArea->Height), float(GNd->TagAngle), GNd->TagVisible),
              RGB(GNd->FillColor.R, GNd->FillColor.G, GNd->FillColor.B), 
              GNd->MirrorX, GNd->MirrorY);
            }
          }

        ModelLink ^ MLnk;
        if (engineProtocol->model->Links->TryGetValue(*guid, MLnk))
          {
          m_pConn->OnModifyLinkM(eventId, requestId, ToCString(MLnk->Guid.ToString()), ToCString(MLnk->Tag), ToCString(MLnk->LinkClass), 
            CSvcLnkMBlk(ToCString(MLnk->Origin.ToString()), ToCString(MLnk->Destination.ToString()), "", "", ToCString(MLnk->OriginPort), ToCString(MLnk->DestinationPort)));
          }

        GraphicLink ^GLnk;
        if (clientProtocol->graphic->Links->TryGetValue(*guid, GLnk))
          {

          CPointFList Pts;
          for each (SysCAD::Protocol::Point ^ Pt in GLnk->ControlPoints)
            {
            Pts.AddTail(CPointF(Pt->X, Pt->Y));
            }
          m_pConn->OnModifyLinkG(eventId, requestId, ToCString(GLnk->Guid.ToString()), ToCString(GLnk->Tag), "", 
            CSvcGuidPair(ToCString(GLnk->Origin.ToString()), ToCString(GLnk->Destination.ToString())),
            Pts, 
            CSvcTagBlk(CRectangleF(GLnk->TagArea->Left, GLnk->TagArea->Top, GLnk->TagArea->Width, GLnk->TagArea->Height), float(GLnk->TagAngle), GLnk->TagVisible));
          }
        }
      }

    void ProcessDeletedList(Int64 eventId, Int64 requestId, Collection<Guid> ^ deleted)
      {
      for each (Guid ^ guid in deleted )
        {
        m_pConn->OnDeleteNodeM(eventId, requestId, ToCString(guid->ToString()));

        m_pConn->OnDeleteNodeG(eventId, requestId, ToCString(guid->ToString()));

        m_pConn->OnDeleteLinkM(eventId, requestId, ToCString(guid->ToString()));

        m_pConn->OnDeleteLinkG(eventId, requestId, ToCString(guid->ToString()));
        }
      }

    void Shutdown()
      {
      delete config;//

      LogNote("CSvcConnectCLRThread", 0, "Shutdown");
      };

    void Load()
      {
      Int64 requestId;
      clientProtocol->Load(requestId);
      };

    void Save()
      {
      Int64 requestId;
      clientProtocol->Save(requestId);
      };

    void LogMessage(DWORD Type, LPCSTR Msg)
      {
      Int64 requestId;
      SysCAD::Log::MessageType T;

      //const DWORD LogFlag_CondClear     = 0x00000001;
      //const DWORD LogFlag_Cond          = 0x00000002;
      //const DWORD LogFlag_RngClear      = 0x00000004;
      //const DWORD LogFlag_Rng           = 0x00000008;
      //const DWORD LogFlag_Fatal         = 0x00000010;
      //const DWORD LogFlag_Stop          = 0x00000020;
      //const DWORD LogFlag_Error         = 0x00000040;
      //const DWORD LogFlag_Warning       = 0x00000080;
      //const DWORD LogFlag_Note          = 0x00000100;
      //const DWORD LogFlag_Cmd           = 0x00000200;
      //const DWORD LogFlag_Separator     = 0x00000400;

      //const DWORD LogFlag_AutoClear     = 0x00001000;
      //const DWORD LogFlag_ClearErrList  = 0x00002000;
      //const DWORD LogFlag_ClearCondList = 0x00004000;
      //const DWORD LogFlag_ClearRngList  = 0x00008000;
      //const DWORD LogFlag_FromPGM       = 0x00010000;
      //const DWORD LogFlag_FromCOM       = 0x00020000;
      //const DWORD LogFlag_FromBlackBox  = 0x00040000;

      if (Type & LogFlag_Note)          T=SysCAD::Log::MessageType::Note;
      else if (Type & LogFlag_Warning)  T=SysCAD::Log::MessageType::Warning;
      else /*if(Type & LogFlag_Error)*/ T=SysCAD::Log::MessageType::Error;

      if (clientProtocol != nullptr)
        clientProtocol->LogMessage(requestId, gcnew String(Msg), T);
      };

  protected:
    SysCAD::Protocol::Config ^ config;
    SysCAD::Protocol::ClientProtocol ^ clientProtocol;
    SysCAD::Protocol::EngineProtocol ^ engineProtocol;

    CSvcConnect   * m_pConn;

    SysCAD::Protocol::Action ^m_Action;
    //Collection<Item^>  ^m_Create;// = gcnew Collection<Item^>; 
    //Collection<Item^>  ^m_Modify;// = gcnew Collection<Item^>;
    //Collection<Guid>   ^m_Delete;// = gcnew Collection<Guid>;  

  };


ref class CSvcConnectCLRThreadGlbl
  {
  public:
    static CSvcConnectCLRThread ^ gs_SrvrThread;// = gcnew CSvcConnectCLRThread;
  };

//========================================================================
//
//
//
//========================================================================

CSvcConnectCLR::CSvcConnectCLR(void)
  {
  }

CSvcConnectCLR::~CSvcConnectCLR(void)
  {
  }

void CSvcConnectCLR::Startup(CSvcConnect * pConn)
  {
  LogNote("CSvcConnectCLR", 0, "Startup");

  m_pConn =  pConn;
  //m_pSrvr->Initialise();

  CSvcConnectCLRThreadGlbl::gs_SrvrThread = gcnew CSvcConnectCLRThread(m_pConn);//.Startup("");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Startup();   

  //System::Threading::S
  };

//<<<<<<< .mine
//bool CSvcConnectCLR::PrepareForUpgrade(LPCSTR projectName, LPCSTR projectPath)
//=======
bool CSvcConnectCLR::ConfigSetup(CSvcConnect * pConn)
  {
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->ConfigSetup();   

  }

bool CSvcConnectCLR::PrepareForUpgrade(LPCSTR projectName, LPCSTR projectPath)
//>>>>>>> .r3251
  {
  String^ projectNameString = gcnew String(projectName);
  String^ projectPathString = gcnew String(projectPath);
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->PrepareForUpgrade(projectNameString, projectPathString);   
  };

void CSvcConnectCLR::Attach2Scd10()
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Attach2Scd10();
  };

void CSvcConnectCLR::Shutdown()
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Shutdown();
  delete CSvcConnectCLRThreadGlbl::gs_SrvrThread;

  //m_pSrvr->Terminate();
  m_pConn = NULL;

  LogNote("CSvcConnectCLR", 0, "Shutdown");
  };

//========================================================================

void CSvcConnectCLR::AddCreateGroup(__int64 & requestId, LPCSTR GrpGuid, LPCSTR Tag, LPCSTR Path, const CRectangleF & boundingRect)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateGroup(requestId, GrpGuid, Tag, Path, boundingRect);
  };

//========================================================================

void CSvcConnectCLR::AddCreateNode(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path,  
                                   LPCSTR ClassId, LPCSTR Symbol, const CRectangleF & boundingRect,
                                   double Angle, const CSvcTagBlk & TagBlk, COLORREF FillColor, 
                                   bool MirrorX, bool MirrorY)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateNode(requestId, ModelGuid, GraphicGuid, Tag, Path, 
    ClassId, Symbol, boundingRect, Angle, TagBlk, FillColor, MirrorX, MirrorY);
  };

void CSvcConnectCLR::AddDeleteNode(__int64 & requestId, LPCSTR GraphicGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddDeleteNode(requestId, GraphicGuid);
  };

void CSvcConnectCLR::AddModifyNodePosition(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyNodePosition(requestId, GraphicGuid, Delta);
  }

void CSvcConnectCLR::AddModifyTagG(__int64 & requestId, LPCSTR GraphicGuid, Pt_3f Delta, const CSvcTagBlk & TagBlk)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyTagG(requestId, GraphicGuid, Delta, TagBlk);
  }

//========================================================================

void CSvcConnectCLR::AddCreateLink(__int64 & requestId, LPCSTR ModelGuid, LPCSTR GraphicGuid, LPCSTR Tag, LPCSTR Path, 
                                   LPCSTR ClassId, 
                                   LPCSTR OriginMdlGuid, LPCSTR DestinationMdlGuid, 
                                   LPCSTR OriginGrfGuid, LPCSTR DestinationGrfGuid, 
                                   LPCSTR OriginPort, LPCSTR DestinationPort, 
                                   CPointFList & ControlPoints, const CSvcTagBlk & TagBlk)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddCreateLink(requestId, ModelGuid, GraphicGuid, Tag, Path, 
    ClassId, 
    OriginMdlGuid, DestinationMdlGuid, 
    OriginGrfGuid, DestinationGrfGuid, 
    OriginPort, DestinationPort, 
    ControlPoints, TagBlk);
  };

void CSvcConnectCLR::AddDeleteLink(__int64 & requestId, LPCSTR GraphicGuid)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddDeleteLink(requestId, GraphicGuid);
  };

void CSvcConnectCLR::AddModifyLink(__int64 & requestId, LPCSTR LinkGuid, LPCSTR Tag, LPCSTR Path, 
                                  LPCSTR ClassId, 
                                  const CSvcGuidPair & Guids, 
                                  LPCSTR OriginPort, LPCSTR DestinationPort, 
                                  CPointFList & ControlPoints, const CSvcTagBlk & TagBlk)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->AddModifyLink(requestId, LinkGuid, Tag, Path, 
    ClassId, 
    Guids, OriginPort, DestinationPort, 
    ControlPoints, TagBlk);
  };


//========================================================================

bool CSvcConnectCLR::ProcessChangeLists(__int64 & requestId)
  {
  return CSvcConnectCLRThreadGlbl::gs_SrvrThread->ProcessChangeLists(requestId);
  }

//========================================================================

void CSvcConnectCLR::Load()
  {
  LogNote("CSvcConnectCLR", 0, "Load");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Load();
  };

void CSvcConnectCLR::Save()
  {
  LogNote("CSvcConnectCLR", 0, "Save");

  CSvcConnectCLRThreadGlbl::gs_SrvrThread->Save();
  };

//========================================================================

void CSvcConnectCLR::LogMessage(DWORD Type, LPCSTR Msg)
  {
  CSvcConnectCLRThreadGlbl::gs_SrvrThread->LogMessage(Type, Msg);
  };

//========================================================================

#endif

//========================================================================
