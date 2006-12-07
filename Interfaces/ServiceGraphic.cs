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
using System.Drawing.Drawing2D;

namespace SysCAD.Interface
{
  [Serializable]
  public sealed class ServiceGraphic : BaseGraphic
  {
    private Int64 requestId;
    private Int64 eventId;

    public delegate bool CreateItemHandler(ServiceGraphic graphic, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool ModifyItemHandler(ServiceGraphic graphic, Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool DeleteItemHandler(ServiceGraphic graphic, Int64 requestId, Guid guid);

    public delegate bool CreateLinkHandler(ServiceGraphic graphic, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool ModifyLinkHandler(ServiceGraphic graphic, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints);
    public delegate bool DeleteLinkHandler(ServiceGraphic graphic, Int64 requestId, Guid guid);

    public delegate bool CreateThingHandler(ServiceGraphic graphic, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool ModifyThingHandler(ServiceGraphic graphic, Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY);
    public delegate bool DeleteThingHandler(ServiceGraphic graphic, Int64 requestId, Guid guid);

    public delegate PortStatus PortCheckHandler(ServiceGraphic graphic, Guid itemGuid, Anchor anchor);

    public delegate ArrayList PropertyListHandler(ServiceGraphic graphic, Guid guid, String tag, String path);

    private CreateItemHandler createItemHandler;
    private ModifyItemHandler modifyItemHandler;
    private DeleteItemHandler deleteItemHandler;

    private CreateLinkHandler createLinkHandler;
    private ModifyLinkHandler modifyLinkHandler;
    private DeleteLinkHandler deleteLinkHandler;

    private CreateThingHandler createThingHandler;
    private ModifyThingHandler modifyThingHandler;
    private DeleteThingHandler deleteThingHandler;

    private PortCheckHandler portCheckHandler;

    private PropertyListHandler propertyListHandler;

    public ServiceGraphic(
      CreateItemHandler createItemHandler, ModifyItemHandler modifyItemHandler, DeleteItemHandler deleteItemHandler,
      CreateLinkHandler createLinkHandler, ModifyLinkHandler modifyLinkHandler, DeleteLinkHandler deleteLinkHandler,
      CreateThingHandler createThingHandler, ModifyThingHandler modifyThingHandler, DeleteThingHandler deleteThingHandler,
      PortCheckHandler portCheckHandler, PropertyListHandler propertyListHandler)
    {
      this.createItemHandler = createItemHandler;
      this.modifyItemHandler = modifyItemHandler;
      this.deleteItemHandler = deleteItemHandler;

      this.createLinkHandler = createLinkHandler;
      this.modifyLinkHandler = modifyLinkHandler;
      this.deleteLinkHandler = deleteLinkHandler;

      this.createThingHandler = createThingHandler;
      this.modifyThingHandler = modifyThingHandler;
      this.deleteThingHandler = deleteThingHandler;

      this.portCheckHandler = portCheckHandler;

      this.propertyListHandler = propertyListHandler;
    }

    public bool CreateItem(out Int64 requestId, out Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      guid = Guid.NewGuid();
      return createItemHandler(this, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyItem(out Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicItems.ContainsKey(guid))
        return modifyItemHandler(this, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, fillMode, mirrorX, mirrorY);
      else
        return false;
    }

    public bool DeleteItem(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicItems.ContainsKey(guid))
        return deleteItemHandler(this, requestId, guid);
      else
        return false;
    }

    public bool CreateLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestId++;
      requestId = this.requestId;
      guid = new Guid();
      return createLinkHandler(this, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
    }

    public bool ModifyLink(out Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicLinks.ContainsKey(guid))
        return modifyLinkHandler(this, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      else
        return false;
    }

    public bool DeleteLink(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicLinks.ContainsKey(guid))
        return deleteLinkHandler(this, requestId, guid);
      else
        return false;
    }


    public bool CreateThing(out Int64 requestId, out Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      guid = Guid.NewGuid();
      return createThingHandler(this, requestId, guid, tag, path, boundingRect, angle, fillColor, elements, decorations, textArea, fillMode, mirrorX, mirrorY);
    }

    public bool ModifyThing(out Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicThings.ContainsKey(guid))
        return modifyThingHandler(this, requestId, guid, tag, path, boundingRect, angle, fillColor, elements, decorations, textArea, fillMode, mirrorX, mirrorY);
      else
        return false;
    }

    public bool DeleteThing(out Int64 requestId, Guid guid)
    {
      this.requestId++;
      requestId = this.requestId;
      if (graphicThings.ContainsKey(guid))
        return deleteThingHandler(this, requestId, guid);
      else
        return false;
    }


    public PortStatus PortCheck(Guid itemGuid, Anchor anchor)
    {
      if (graphicItems.ContainsKey(itemGuid))
        return portCheckHandler(this, itemGuid, anchor);
      else
        return PortStatus.Unavailable;
    }


    public ArrayList PropertyList(Guid guid, String tag, String path)
    {
      //todo: check path is valid.
      return propertyListHandler(this, guid, tag, path);
    }


    public void DoItemCreated(Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      if (!graphicItems.ContainsKey(guid))
      {
        GraphicItem graphicItem = new GraphicItem(guid, tag);
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        graphicItems.Add(guid, graphicItem);

        eventId++;
        OnItemCreated(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoItemModified(Int64 requestId, Guid guid, String tag, String path, Model model, Shape stencil, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      GraphicItem graphicItem;
      if (graphicItems.TryGetValue(guid, out graphicItem))
      {
        graphicItem.Tag = tag;
        graphicItem.Path = path;
        graphicItem.Model = model;
        graphicItem.Shape = stencil;
        graphicItem.BoundingRect = (ARectangleF)boundingRect;
        graphicItem.Angle = angle;
        graphicItem.FillColor = fillColor;
        graphicItem.FillMode = fillMode;
        graphicItem.MirrorX = mirrorX;
        graphicItem.MirrorY = mirrorY;

        eventId++;
        OnItemModified(eventId, requestId, guid, tag, path, model, stencil, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoItemDeleted(Int64 requestId, Guid guid)
    {
      if (graphicItems.ContainsKey(guid))
      {
        graphicItems.Remove(guid);

        eventId++;
        OnItemDeleted(eventId, requestId, guid);
      }
    }

    public void DoLinkCreated(Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      if (!graphicItems.ContainsKey(guid))
      {
        GraphicLink graphicLink = new GraphicLink(guid, tag, classId, origin, originPort, destination, destinationPort);

        graphicLink.controlPoints = new List<PointF>();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        graphicLinks.Add(guid, graphicLink);

        eventId++;
        OnLinkCreated(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void DoLinkModified(Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<PointF> controlPoints)
    {
      GraphicLink graphicLink;
      if (graphicLinks.TryGetValue(guid, out graphicLink))
      {
        graphicLink.Tag = tag;
        graphicLink.ClassID = classId;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        graphicLink.OriginPort = originPort;
        graphicLink.DestinationPort = destinationPort;

        graphicLink.controlPoints.Clear();
        foreach (PointF controlPoint in controlPoints)
          graphicLink.controlPoints.Add(controlPoint);

        eventId++;
        OnLinkModified(eventId, requestId, guid, tag, classId, origin, destination, originPort, destinationPort, controlPoints);
      }
    }

    public void DoLinkDeleted(Int64 requestId, Guid guid)
    {
      if (graphicLinks.ContainsKey(guid))
      {
        graphicLinks.Remove(guid);

        eventId++;
        OnLinkDeleted(eventId, requestId, guid);
      }
    }


    public void DoThingCreated(Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      if (!graphicThings.ContainsKey(guid))
      {
        GraphicThing graphicThing = new GraphicThing(guid, tag);
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Angle = angle;
        graphicThing.FillColor = fillColor;
        graphicThing.Elements = elements;
        graphicThing.Decorations = decorations;
        graphicThing.TextArea = textArea;
        graphicThing.FillMode = fillMode;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        graphicThings.Add(guid, graphicThing);

        eventId++;
        OnThingCreated(eventId, requestId, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoThingModified(Int64 requestId, Guid guid, String tag, String path, RectangleF boundingRect, Single angle, System.Drawing.Color fillColor, ArrayList elements, ArrayList decorations, ArrayList textArea, System.Drawing.Drawing2D.FillMode fillMode, bool mirrorX, bool mirrorY)
    {
      GraphicThing graphicThing;
      if (graphicThings.TryGetValue(guid, out graphicThing))
      {
        graphicThing.Tag = tag;
        graphicThing.Path = path;
        graphicThing.BoundingRect = (ARectangleF)boundingRect;
        graphicThing.Angle = angle;
        graphicThing.FillColor = fillColor;
        graphicThing.Elements = elements;
        graphicThing.Decorations = decorations;
        graphicThing.TextArea = textArea;
        graphicThing.FillMode = fillMode;
        graphicThing.MirrorX = mirrorX;
        graphicThing.MirrorY = mirrorY;

        eventId++;
        OnThingModified(eventId, requestId, guid, tag, path, boundingRect, angle, fillColor, mirrorX, mirrorY);
      }
    }

    public void DoThingDeleted(Int64 requestId, Guid guid)
    {
      if (graphicThings.ContainsKey(guid))
      {
        graphicThings.Remove(guid);

        eventId++;
        OnThingDeleted(eventId, requestId, guid);
      }
    }

  }
}