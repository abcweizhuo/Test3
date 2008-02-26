
using System;
using System.IO;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Soap;
using System.Runtime.Remoting.Lifetime;
using System.Runtime.Remoting.Channels;
using System.Collections;
using System.Runtime.Serialization.Formatters;
using System.Runtime.Remoting.Channels.Tcp;

//using System.Security.Permissions;

namespace SysCAD.Protocol
{

  public class ConfigData : MarshalByRefObject
  {

    private Dictionary<String, GraphicStencil> graphicStencils = new Dictionary<String, GraphicStencil>();

    private Dictionary<String, ModelStencil> modelStencils = new Dictionary<String, ModelStencil>();
    private StringCollection projectList = new StringCollection();
    private Dictionary<String, ThingStencil> thingStencils = new Dictionary<String, ThingStencil>();

    public delegate bool AddProjectHandler(string name, string path);
    public delegate void AddProjectAnywayHandler(string name, string path);

    public AddProjectHandler addProject;
    public AddProjectAnywayHandler addProjectAnyway;

    public void Setup(string stencilPath, Config.AddProjectHandler addProject, Config.AddProjectAnywayHandler addProjectAnyway)
    {
      this.addProject = addProject;
      this.addProjectAnyway = addProjectAnyway;

      //CreateDummyModelStencil(stencilPath + "\\Example.ModelStencil");
      //CreateDummyGraphicStencil(stencilPath + "\\Example.GraphicStencil");
      //CreateDummyThingStencil(stencilPath + "\\Example.ThingStencil");

      {
        //int iStencil = 0;
        //LogNote("Srvr : 0 : ModelStencils:");
        String[] files = Directory.GetFiles(stencilPath, "*.modelstencil");
        for (int i = 0; i < files.GetLength(0); i++)
        {
          String fullpath = files[i];

          //try
          //{
          ModelStencil modelStencil = ModelStencil.Deserialize(fullpath);

          if (ConfirmModelStencil(modelStencil))
          {
            TrimAnchorPoints(modelStencil);
            ModelStencils.Add(System.IO.Path.GetFileNameWithoutExtension(fullpath), modelStencil);
          }

          //Console.WriteLine("  {0}] {1}", iStencil++, Path.GetFileNameWithoutExtension(fullpath));
          //LogNote("Srvr : 0 : " + iStencil++ + " : " + System.IO.Path.GetFileNameWithoutExtension(fullpath));
          //}
          //catch (Exception)
          //{
          //  int asdf = 0;
          //  //Message("Error '" + e.Message + "' loading ModelStencil: " + fullpath, MessageType.Error);
          //}
        }

        Console.WriteLine("\n");
      }

      {
        //int iStencil = 0;
        //LogNote("Srvr : 0 : GraphicStencils:");
        String[] files = Directory.GetFiles(stencilPath, "*.graphicstencil");
        for (int i = 0; i < files.GetLength(0); i++)
        {
          String fullpath = files[i];

          //try
          //{
          GraphicStencil graphicStencil = GraphicStencil.Deserialize(fullpath);

          GraphicStencils.Add(System.IO.Path.GetFileNameWithoutExtension(fullpath).ToLower().Replace(' ', '_'), graphicStencil);
          //}
          //catch
          //{
          //  int adsf = 0;
          //  //logNote("Config : Error loading GraphicStencil " + fullpath);
          //}
        }
      }

      //{
      //  int iStencil = 0;
      //  LogNote("Srvr : 0 : ThingStencils:");
      //  String[] files = Directory.GetFiles(stencilPath, "*.thingstencil");
      //  for (int i = 0; i < files.GetLength(0); i++)
      //  {
      //    String fullpath = files[i];

      //    SoapFormatter sf = new SoapFormatter();
      //    StreamReader streamRdr = new StreamReader(fullpath);
      //    Stream stream = streamRdr.BaseStream;
      //    ThingStencil thingStencil = (ThingStencil)sf.Deserialize(stream);
      //    thingStencil.Tag = System.IO.Path.GetFileNameWithoutExtension(fullpath);
      //    configData.ThingStencils.Add(System.IO.Path.GetFileNameWithoutExtension(fullpath), thingStencil);
      //    stream.Close();
      //    LogNote("Srvr : 0 : " + iStencil++ + " : " + System.IO.Path.GetFileNameWithoutExtension(fullpath));
      //  }
      //}
    }

    bool ConfirmModelStencil(ModelStencil modelstencil)
    {
      // TODO: check whether this stencil is to be included in the project.
      return true;
    }

    void TrimAnchorPoints(ModelStencil modelStencil)
    {
      ArrayList anchors = modelStencil.Anchors;
      ArrayList validAnchors = new ArrayList();
      for (int i = 0; i < anchors.Count; i++)
      {
        Anchor anchor = (Anchor)(anchors[i]);
        if (ValidAnchor(anchor))
        {
          validAnchors.Add(anchor);
        }
      }

      modelStencil.Anchors = validAnchors;
    }

    bool ValidAnchor(Anchor anchor)
    {
      // TODO: Check here if anchor.Tag is to be included. (and possibly in future anchor.Type)
      return true;
    }

    private static void CreateDummyThingStencil(String fullpath)
    {
      //Create dummy ThingStencil for comparison...
      {
        ThingStencil thingStencil = new ThingStencil();
        thingStencil.Tag = "a tank annotation";
        thingStencil.DefaultSize.Width = 20.0;
        thingStencil.DefaultSize.Height = 20.0;
        thingStencil.Xaml =
            "            <!-- Saved from Aurora XAML Designer for WinFX - Mobiform Software Ltd. - Thursday, 4 January 2007 11:45:44 AM -." +
            "<Canvas xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\" Name=\"Canvas1\" Width=\"132\" Height=\"141\" Background=\"{x:Static Brushes.Transparent}\" xmlns:x=\"http://schemas.microsoft.com/winfx/2006/xaml\">" +
            "  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"105\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0.03,1\">" +
            "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Rectangle Canvas.Left=\"6\" Canvas.Top=\"21\" Width=\"120\" Height=\"100\" Stroke=\"Gray\" StrokeThickness=\"0.5\">" +
            "    <Rectangle.Fill>" +
            "      <LinearGradientBrush StartPoint=\"0,1\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Rectangle.Fill>" +
            "  </Rectangle>" +
            "  <Rectangle Canvas.Left=\"16\" Canvas.Bottom=\"21\" Width=\"100\" Height=\"100\" Fill=\"Blue\" Stroke=\"Black\">" +
            "  </Rectangle>" +
            "  <Rectangle Canvas.Left=\"16\" Canvas.Top=\"31\" Width=\"100\" Height=\"[[[[TAG]], 0, 3, 0, 85, Linear, Integer]]\" Stroke=\"Black\">" +
            "    <Rectangle.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"Black\" Offset=\"0\" />" +
            "        <GradientStop Color=\"Black\" Offset=\"1\" />" +
            "        <GradientStop Color=\"#FF444444\" Offset=\"0.821339950372208\" />" +
            "      </LinearGradientBrush>" +
            "    </Rectangle.Fill>" +
            "  </Rectangle>" +
            "  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"White\" Offset=\"0.5\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Ellipse Canvas.Left=\"2\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"White\" Offset=\"0.5\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"129\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"White\" Offset=\"0.5\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"63\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"White\" Offset=\"0.5\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Ellipse Canvas.Left=\"120.5\" Canvas.Top=\"34\" Width=\"10\" Height=\"20\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"1,0\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"White\" Offset=\"0.5\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Ellipse Canvas.Left=\"56\" Canvas.Top=\"2\" Width=\"20\" Height=\"10\" Stroke=\"#FF716F64\">" +
            "    <Ellipse.Fill>" +
            "      <LinearGradientBrush Opacity=\"1\" StartPoint=\"0,1\" EndPoint=\"1,1\">" +
            "        <GradientStop Color=\"#FFD4D0C8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"White\" Offset=\"0.5\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </LinearGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "  <Ellipse Canvas.Left=\"6\" Canvas.Top=\"7\" Width=\"120\" Height=\"30\" Stroke=\"#FF716F64\" StrokeThickness=\"0.5\">" +
            "    <Ellipse.Fill>" +
            "      <RadialGradientBrush Opacity=\"1\" Center=\"0.490074441687345,0.5\" RadiusX=\"0.52\" RadiusY=\"0.599255583126551\" GradientOrigin=\"0.16,0.54\">" +
            "        <GradientStop Color=\"#FFECE9D8\" Offset=\"0\" />" +
            "        <GradientStop Color=\"#FF716F64\" Offset=\"1\" />" +
            "      </RadialGradientBrush>" +
            "    </Ellipse.Fill>" +
            "  </Ellipse>" +
            "</Canvas>";

        ThingStencil.Serialize(fullpath, thingStencil);
      }
    }

    private static void CreateDummyGraphicStencil(String fullpath)
    {
      //Create dummy GraphicStencil for comparison...
      {
        GraphicStencil graphicStencil2 = new GraphicStencil();
        graphicStencil2.Tag = "";
        ArrayList elements = new ArrayList();
        SysCAD.Protocol.Arc arc = new SysCAD.Protocol.Arc(0, 0, 100, 100, 10, 360);
        graphicStencil2.TagArea = new SysCAD.Protocol.Rectangle(0, 0, 100, 100);
        elements.Add(arc);
        graphicStencil2.Elements = elements;

        graphicStencil2.Decorations = new ArrayList();

        GraphicStencil.Serialize(fullpath, graphicStencil2);
      }
    }

    private static void CreateDummyModelStencil(String fullpath)
    {
      //Create dummy ModelStencil for comparison...
      {
        ModelStencil modelStencil2 = new ModelStencil();
        modelStencil2.Tag = "";
        modelStencil2.Groups = new ArrayList();
        modelStencil2.Groups.Add("Control");
        ArrayList elements = new ArrayList();
        SysCAD.Protocol.Arc arc = new SysCAD.Protocol.Arc(0, 0, 100, 100, 10, 360);
        elements.Add(arc);
        modelStencil2.Elements = elements;

        modelStencil2.Decorations = new ArrayList();
        ArrayList anchors = new ArrayList();
        Anchor anchor = new Anchor("tag", AnchorType.Electrical, 0, 0.0f, 1.0f);
        anchors.Add(anchor);
        Anchor anchor2 = new Anchor("tag2", AnchorType.Electrical, 0, 0.0f, 1.0f);
        anchors.Add(anchor2);
        modelStencil2.Anchors = anchors;

        ModelStencil.Serialize(fullpath, modelStencil2);
      }
    }

    //[SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }

    public Dictionary<String, GraphicStencil> GraphicStencils
    {
      get { return graphicStencils; }
      set { this.graphicStencils = value; }
    }

    public Dictionary<String, ModelStencil> ModelStencils
    {
      get { return modelStencils; }
      set { this.modelStencils = value; }
    }

    public StringCollection ProjectList
    {
      get { return projectList; }
      set { projectList = value; }
    }

    public Dictionary<String, ThingStencil> ThingStencils
    {
      get { return thingStencils; }
      set { this.thingStencils = value; }
    }
  }

  public class Config : ConfigData
  {
    public String connectionError = "";

    private ConfigData remoteConfig;

    public void GetProjectList()
    {
      ProjectList = remoteConfig.ProjectList;
    }

    public bool RemoteAddProject(string name, string path)
    {
      return remoteConfig.addProject(name, path);
    }

    public void RemoteAddProjectAnyway(string name, string path)
    {
      remoteConfig.addProjectAnyway(name, path);
    }

    public void Syncxxx()
    {
      MemoryStream memoryStream;
      System.Runtime.Serialization.Formatters.Binary.BinaryFormatter bf = new System.Runtime.Serialization.Formatters.Binary.BinaryFormatter();

      ProjectList = remoteConfig.ProjectList;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ModelStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ModelStencils = bf.Deserialize(memoryStream) as Dictionary<String, ModelStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.GraphicStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      GraphicStencils = bf.Deserialize(memoryStream) as Dictionary<String, GraphicStencil>;

      memoryStream = new MemoryStream();
      bf.Serialize(memoryStream, remoteConfig.ThingStencils);
      memoryStream.Seek(0, SeekOrigin.Begin);
      ThingStencils = bf.Deserialize(memoryStream) as Dictionary<String, ThingStencil>;

      //addProject += remoteConfig.addProject;
    }

    public bool TestUrl(Uri url)
    {
      try
      {
        remoteConfig = Activator.GetObject(typeof(ConfigData), url.ToString()) as ConfigData;
        StringCollection test = remoteConfig.ProjectList; // Check that the connection is up.
        return (remoteConfig != null);
      }
      catch (System.Runtime.Remoting.RemotingException remotingException)
      {
        String connectionError = remotingException.Message;
        return false;
      }
    }
  }
}
