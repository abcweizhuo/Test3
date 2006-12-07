using System;
using System.Collections.Generic;
using System.Runtime.Serialization.Formatters.Binary;
using System.IO;

using System.Runtime.Remoting.Lifetime;
using System.Drawing;
using System.Threading;

using System.Runtime.Remoting.Channels.Ipc;
using System.Runtime.Remoting.Channels;
using System.Collections;
using System.Runtime.Remoting.Channels.Tcp;
using System.Security.Permissions;

namespace SysCAD.Interface
{
  [Serializable]
  public class BaseModel : MarshalByRefObject
  {
    private String name;

    public Dictionary<Guid, GraphicLink> modelLinks;
    public Dictionary<Guid, GraphicItem> modelItems;

    public BaseModel()
    {
      modelLinks = new Dictionary<Guid, GraphicLink>();
      modelItems = new Dictionary<Guid, GraphicItem>();
    }

    public String Name
    {
      get { return name; }
      set { name = value; }
    }



    public delegate void ItemCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY);
    public delegate void ItemDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);


    public delegate void LinkCreatedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkModifiedHandler(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate void LinkDeletedHandler(Int64 eventId, Int64 requestId, Guid guid);



    public ItemCreatedHandler ItemCreated;
    public ItemModifiedHandler ItemModified;
    public ItemDeletedHandler ItemDeleted;


    public LinkCreatedHandler LinkCreated;
    public LinkModifiedHandler LinkModified;
    public LinkDeletedHandler LinkDeleted;



    public void OnItemCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemCreated != null)
        ItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnItemModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, bool mirrorX, bool mirrorY)
    {
      if (ItemModified != null)
        ItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
    }

    public void OnItemDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (ItemDeleted != null)
        ItemDeleted(eventId, requestId, guid);
    }



    public void OnLinkCreated(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkCreated != null)
        LinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void OnLinkModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (LinkModified != null)
        LinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public void OnLinkDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      if (LinkDeleted != null)
        LinkDeleted(eventId, requestId, guid);
    }


    [SecurityPermission(SecurityAction.LinkDemand, Flags = SecurityPermissionFlag.Infrastructure)]
    public override Object InitializeLifetimeService()
    {
      return null;
    }
  }
}