
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;

using MindFusion.FlowChartX;

using SysCAD.Protocol;
using PureComponents.TreeView;
using System.Windows.Forms;
using ActiproSoftware.UIStudio.Bar;
using MindFusion.FlowChartX.LayoutSystem;
using System.Drawing.Drawing2D;
using System.Collections;
using System.Globalization;
using System.Resources;
using System.Reflection;
using System.Collections.ObjectModel;
using System.Timers;

[assembly: NeutralResourcesLanguageAttribute("en-US")]
namespace SysCAD.Editor
{

  internal partial class FrmFlowChart : Form
  {
    private SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();
    private System.Timers.Timer actionExpire = new System.Timers.Timer(500.0);

    private Int32 tempTagExtension; // = 0;

    private Arrow arrowBeingModified;
    private int arrowBeingModifiedSelectionHandle = -1;

    private String currentModel;
    private String currentStencil;
    private Anchor destinationAnchorChosen;

    private EditorForm form1;

    private EditorLink hoverLink;
    private EditorNode hoverNode;
    private EditorArea hoverArea;
    private PointF hoverPoint;

    private EditorNode contextNode;
    private EditorArea contextArea;

    ResourceManager stringManager;

    int newDestinationAnchor = -1;
    Box newDestinationBox;

    Guid newDestinationGuid = Guid.Empty;
    String newDestinationTag = String.Empty;
    int newOriginAnchor = -1;
    Box newOriginBox;

    Guid newOriginGuid = Guid.Empty;
    String newOriginTag = String.Empty;

    List<SysCAD.Protocol.Point> oldControlPoints = new List<SysCAD.Protocol.Point>();
    int oldDestinationAnchor = -1;
    Box oldDestinationBox;

    Guid oldDestinationGuid = Guid.Empty;

    private EditorLink oldHoverLink;
    private EditorNode oldHoverNode;
    int oldOriginAnchor = -1;
    Box oldOriginBox;

    Guid oldOriginGuid = Guid.Empty;

    private Anchor originAnchorChosen;

    public State state = new State();
    private int tempArrowKey;

    //private int tempBoxKey;

    private Int64 requestId;

    public FrmFlowChart(EditorForm form1)
    {
      stringManager = new ResourceManager("en-US", Assembly.GetExecutingAssembly());

      actionExpire.Elapsed += new ElapsedEventHandler(DoChange);

      this.form1 = form1;
      InitializeComponent();

      this.fcFlowChart.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.fcFlowChart_MouseWheel);

      fcFlowChart.DocExtents = new RectangleF(0.0F, 0.0F, 297.0F, 210.0F);
      fcFlowChart.Selection.Style = SelectionStyle.SemiTransparent;
    }

    public void FixDocExtents()
    {
      float minX = float.MaxValue, minY = float.MaxValue;
      float maxX = -float.MaxValue, maxY = -float.MaxValue;

      bool foundObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        foundObject = true;

        if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;

        if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;

        if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;

        if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        foundObject = true;

        if (arrow.BoundingRect.Left < minX) minX = arrow.BoundingRect.Left;

        if (arrow.BoundingRect.Top < minY) minY = arrow.BoundingRect.Top;

        if (arrow.BoundingRect.Right > maxX) maxX = arrow.BoundingRect.Right;

        if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundObject)
        fcFlowChart.DocExtents = RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F);
    }

    public void SetSizes()
    {
      float zoomFactor = fcFlowChart.ZoomFactor;

      fcFlowChart.SelHandleSize = 100.0F / zoomFactor;
      fcFlowChart.MergeThreshold = 100.0F / zoomFactor;

      fcFlowChart.ArrowHeadSize = 150.0F / zoomFactor;

      if (80.0F / zoomFactor > getMinArrowheadSize(fcFlowChart.MeasureUnit))
        fcFlowChart.ArrowIntermSize = 80.0F / zoomFactor;

      else
        fcFlowChart.ArrowIntermSize = getMinArrowheadSize(fcFlowChart.MeasureUnit);
      fcFlowChart.ArrowBaseSize = 150.0F / zoomFactor;

      foreach (EditorLink link in state.Links)
      {
        link.Arrow.ArrowHeadSize = 150.0F / zoomFactor;

        if (80.0F / zoomFactor > getMinArrowheadSize(fcFlowChart.MeasureUnit))
          link.Arrow.IntermHeadSize = 120.0F / zoomFactor;

        else
          link.Arrow.IntermHeadSize = getMinArrowheadSize(fcFlowChart.MeasureUnit);
        link.Arrow.ArrowBaseSize = 150.0F / zoomFactor;
      }

      System.Drawing.Font standardFont = new System.Drawing.Font("Microsoft Sans Serif", zoomFactor / 10.0F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));

      fcFlowChart.Font = standardFont;

      foreach (EditorNode item in state.Nodes)
      {
        item.TextBox.Font = item.GraphicNode.TagFont.ScaledFont(zoomFactor);
      }

      foreach (EditorLink link in state.Links)
      {
        link.TextBox.Font = link.GraphicLink.TagFont.ScaledFont(zoomFactor);
      }

      fcFlowChart.Invalidate();
    }

    public void ZoomToSelected()
    {
      FixDocExtents();

      float minX = float.MaxValue, minY = float.MaxValue;
      float maxX = -float.MaxValue, maxY = -float.MaxValue;

      bool foundSelectedObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {

        if (box.Selected)
        {
          foundSelectedObject = true;

          if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;

          if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;

          if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;

          if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {

        if (arrow.Selected)
        {
          foundSelectedObject = true;

          if (arrow.BoundingRect.Left < minX) minX = arrow.BoundingRect.Left;

          if (arrow.BoundingRect.Top < minY) minY = arrow.BoundingRect.Top;

          if (arrow.BoundingRect.Right > maxX) maxX = arrow.BoundingRect.Right;

          if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundSelectedObject)
        fcFlowChart.ZoomToRect(RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F));

      else
        fcFlowChart.ZoomToRect(fcFlowChart.DocExtents);
    }

    public void ZoomToVisible()
    {
      FixDocExtents();

      float minX = float.MaxValue, minY = float.MaxValue;
      float maxX = -float.MaxValue, maxY = -float.MaxValue;

      bool foundVisibleObject = false;

      foreach (Box box in fcFlowChart.Boxes)
      {
        if (box.Tag is EditorNode)
        {
          EditorNode node = box.Tag as EditorNode;
          if (node.Visible)
          {
            foundVisibleObject = true;
            if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;
            if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;
            if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;
            if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
          }
        }
        else if (box.Tag is EditorArea)
        {
          EditorArea area = box.Tag as EditorArea;
          if (area.Visible)
          {
            foundVisibleObject = true;
            if (box.BoundingRect.Left < minX) minX = box.BoundingRect.Left;
            if (box.BoundingRect.Top < minY) minY = box.BoundingRect.Top;
            if (box.BoundingRect.Right > maxX) maxX = box.BoundingRect.Right;
            if (box.BoundingRect.Bottom > maxY) maxY = box.BoundingRect.Bottom;
          }
        }
      }

      foreach (Arrow arrow in fcFlowChart.Arrows)
      {
        if (arrow.Tag is EditorLink)
        {
          EditorLink link = arrow.Tag as EditorLink;
          if (link.Visible)
          {
            foundVisibleObject = true;
            if (arrow.BoundingRect.Left < minX) minX = arrow.BoundingRect.Left;
            if (arrow.BoundingRect.Top < minY) minY = arrow.BoundingRect.Top;
            if (arrow.BoundingRect.Right > maxX) maxX = arrow.BoundingRect.Right;
            if (arrow.BoundingRect.Bottom > maxY) maxY = arrow.BoundingRect.Bottom;
          }
        }
      }

      float width = maxX - minX;
      float height = maxY - minY;

      if (foundVisibleObject)
        fcFlowChart.ZoomToRect(RectangleF.FromLTRB(minX - width * 0.05F, minY - height * 0.05F, maxX + width * 0.05F, maxY + height * 0.05F));

      else
        fcFlowChart.ZoomToRect(fcFlowChart.DocExtents);

      SetSizes();
    }

    internal static SysCAD.Protocol.Point getCenter(SysCAD.Protocol.Rectangle rect)
    {
      return new SysCAD.Protocol.Point(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);
    }

    internal static float getMinArrowheadSize(GraphicsUnit currUnit)
    {

      switch (currUnit)
      {

        case GraphicsUnit.Millimeter:
          return 1;

        case GraphicsUnit.Inch:
          return 1.0f / 12;

        case GraphicsUnit.Point:
          return 72.0f / 12;

        case GraphicsUnit.Pixel:
          return 4;

        case GraphicsUnit.Document:
          return 300.0f / 12;

        case GraphicsUnit.Display:
          return 75.0f / 12;
        default:
          return 0.0F;
      }
    }

    internal bool MergePoints(SysCAD.Protocol.Point point1, SysCAD.Protocol.Point point2)
    {
      return
        Math.Abs(point1.X - point2.X) <= fcFlowChart.MergeThreshold &&
        Math.Abs(point1.Y - point2.Y) <= fcFlowChart.MergeThreshold;
    }

    internal static SysCAD.Protocol.Point rotatePointAt(SysCAD.Protocol.Point point, SysCAD.Protocol.Point pivot, Double angle)
    {
      PointF[] points = new PointF[] { point.ToPointF() };
      Matrix rotation = new Matrix();
      rotation.RotateAt((float)angle, pivot.ToPointF());
      rotation.TransformPoints(points);
      rotation.Dispose();
      return new SysCAD.Protocol.Point(points[0]);
    }

    internal void SetProject(ClientProtocol clientProtocol, Config config, PureComponents.TreeView.TreeView tvNavigation)
    {
      state.FlowChart = fcFlowChart;
      state.ClientProtocol = clientProtocol;
      state.Config = config;
      state.TVNavigation = tvNavigation;

      state.ConnectGraphic(
        new ClientProtocol.PermissionsChangedHandler(fcFlowChart_PermissionsChanged),
        new ClientProtocol.StepHandler(fcFlowChart_Step),
        new ClientProtocol.SyncHandler(fcFlowChart_Sync),
        new ClientProtocol.ChangedHandler(fcFlowChart_Changed),
        new ClientProtocol.PortInfoRequestedHandler(fcFlowChart_PortInfoRequested));

      fcFlowChart.UndoManager.UndoEnabled = false;
      fcFlowChart.UseWaitCursor = true;
      fcFlowChart.ValidityChecks = false;
      fcFlowChart.Visible = false;
      fcFlowChart.Enabled = false;



      foreach (GraphicArea graphicArea in clientProtocol.graphic.Areas.Values)
      {
        state.CreateArea(graphicArea);
      }

      foreach (GraphicNode graphicNode in clientProtocol.graphic.Nodes.Values)
      {
        ModelNode modelNode;
        if (state.ClientProtocol.model.Nodes.TryGetValue(graphicNode.ModelGuid, out modelNode))
        {
          state.CreateNode(modelNode, graphicNode);
        }
        else
        {
          state.CreateNode(null, graphicNode);
          //state.ClientProtocol.LogMessage(out requestId, "ModelNode missing for graphicLink (Tag: " + graphicNode.Tag + ", Guid: " + graphicNode.Guid + ")", SysCAD.Log.MessageType.Error);
        }
      }

      foreach (GraphicLink graphicLink in clientProtocol.graphic.Links.Values)
      {
        ModelLink modelLink;
        if (state.ClientProtocol.model.Links.TryGetValue(graphicLink.ModelGuid, out modelLink))
        {
          state.CreateLink(modelLink, graphicLink);
        }
        else
        {
          state.ClientProtocol.LogMessage(out requestId, "ModelLink missing for graphicLink (Tag: " + graphicLink.Tag + ", Guid: " + graphicLink.Guid + ")", SysCAD.Log.MessageType.Error);
        }
      }

      fcFlowChart.UndoManager.UndoEnabled = true;
      fcFlowChart.UseWaitCursor = false;
      fcFlowChart.ValidityChecks = true;
      fcFlowChart.Visible = true;
      fcFlowChart.Enabled = true;
    }

    internal void UnSetProject()
    {
      state.DisconnectGraphic(
          fcFlowChart_PermissionsChanged,
          fcFlowChart_Step,
          fcFlowChart_Sync,
          fcFlowChart_Changed,
          fcFlowChart_PortInfoRequested);
    }

    private void DisconnectDestination(object sender, EventArgs e)
    {
      Arrow hoverArrow = hoverLink.Arrow;

      hoverArrow.DestAnchor = -1;
      hoverArrow.Destination = fcFlowChart.Dummy;

      State.SetControlPoints(hoverArrow, (hoverArrow.Tag as EditorLink).GraphicLink.ControlPoints);
    }

    private void DisconnectOrigin(object sender, EventArgs e)
    {
      Arrow hoverArrow = hoverLink.Arrow;

      hoverArrow.OrgnAnchor = -1;
      hoverArrow.Origin = fcFlowChart.Dummy;

      State.SetControlPoints(hoverArrow, (hoverArrow.Tag as EditorLink).GraphicLink.ControlPoints);
    }

    static private Double Distance(SysCAD.Protocol.Point a, SysCAD.Protocol.Point b)
    {
      return Math.Sqrt(((a.X - b.X) * (a.X - b.X)) + ((a.Y - b.Y) * (a.Y - b.Y)));
    }

    private void DoArrowModifyingOperations(Arrow arrow, int selectionHandle)
    {
      oldControlPoints = State.GetControlPoints(arrow.ControlPoints);

      arrowBeingModifiedSelectionHandle = selectionHandle;
      arrowBeingModified = arrow;
      //arrowBeingModified.CustomDraw = CustomDraw.Additional;
      arrowBeingModified.ZTop();

      if (arrow.Tag != null)
        oldOriginGuid = (arrow.Tag as EditorLink).GraphicLink.Origin;
      else
        oldOriginGuid = Guid.Empty;

      oldOriginBox = arrow.Origin as Box;
      oldOriginAnchor = arrow.OrgnAnchor;

      if (arrow.Tag != null)
        oldDestinationGuid = (arrow.Tag as EditorLink).GraphicLink.Destination;
      else
        oldDestinationGuid = Guid.Empty;

      oldDestinationBox = arrow.Destination as Box;
      oldDestinationAnchor = arrow.DestAnchor;

      if (state.Permissions.Modify)
      {
        if (arrowBeingModifiedSelectionHandle == 0)
        {
          SysCAD.Protocol.Point originPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[0]);
          Box originBox = fcFlowChart.GetBoxAt(originPos.ToPointF(), 2.0F);

          if ((selectionHandle == 0) && (originBox != null) && (!(arrowBeingModified.Origin is Box)))
          {
            originBox = (originBox.Tag as EditorNode).ModelBox;

            if (originBox != null)
            {
              int closestI = 0;
              Double closestDistance = Double.MaxValue;

              for (int i = 0; i < originBox.AnchorPattern.Points.Count; i++)
              {

                if (originBox.AnchorPattern.Points[i].AllowOutgoing)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
                    originBox.AnchorPattern.Points[i].X,
                    originBox.AnchorPattern.Points[i].Y,
                    originBox.RotationAngle);
                  Double thisDistance = Distance(originPos, anchorPointPos);

                  if (thisDistance < closestDistance)
                  {
                    closestDistance = thisDistance;
                    closestI = i;
                  }
                }
              }

              newOriginGuid = (originBox.Tag as EditorNode).Guid;
              newOriginTag = (originBox.Tag as EditorNode).Tag;
              newOriginBox = originBox;
              newOriginAnchor = closestI;

              String anchorString;
              (newOriginBox.Tag as EditorNode).anchorIntToTag.TryGetValue(newOriginAnchor, out anchorString);

              if (anchorString != null)
              {
                form1.ToolStripStatusLabel.Text = "Origin Item: " + newOriginTag +
      " : " + " Oritin Port: " + anchorString.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
              }
            }
          }
        }
        //else if (oldOriginBox != null)
        //{
        //  (e.Arrow.Tag as Link).graphicLink.Origin = oldOriginGuid;
        //  e.Arrow.Origin = oldOriginBox;
        //  e.Arrow.OrgnAnchor = oldOriginAnchor;
        //}

        if (selectionHandle == arrowBeingModified.ControlPoints.Count - 1)
        {
          SysCAD.Protocol.Point destinationPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[arrowBeingModified.ControlPoints.Count - 1]);
          Box destinationBox = fcFlowChart.GetBoxAt(destinationPos.ToPointF(), 2.0F);

          if ((destinationBox != null) && (destinationBox.Tag != null) && (!(arrowBeingModified.Destination is Box)))
          {
            destinationBox = (destinationBox.Tag as EditorNode).ModelBox;

            if (destinationBox != null)
            {
              int closestI = 0;
              Double closestDistance = Double.MaxValue;

              for (int i = 0; i < destinationBox.AnchorPattern.Points.Count; i++)
              {

                if (destinationBox.AnchorPattern.Points[i].AllowIncoming)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(destinationBox.BoundingRect),
                    destinationBox.AnchorPattern.Points[i].X,
                    destinationBox.AnchorPattern.Points[i].Y,
                    destinationBox.RotationAngle);
                  Double thisDistance = Distance(destinationPos, anchorPointPos);

                  if (thisDistance < closestDistance)
                  {
                    closestDistance = thisDistance;
                    closestI = i;
                  }
                }
              }

              newDestinationGuid = (destinationBox.Tag as EditorNode).Guid;
              newDestinationTag = (destinationBox.Tag as EditorNode).Tag;
              newDestinationBox = destinationBox;
              newDestinationAnchor = closestI;

              String anchorString;
              (newDestinationBox.Tag as EditorNode).anchorIntToTag.TryGetValue(newDestinationAnchor, out anchorString);

              if (anchorString != null)
              {
                form1.ToolStripStatusLabel.Text = "Destination Item: " + newDestinationTag +
                  " : " + " Destination Port: " + anchorString.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
              }
            }
          }
        }
      }

      if ((newOriginBox != null) && (newDestinationBox != null) && (newOriginBox == newDestinationBox)) // refliexive, disconnect modifying end.
      {

        if (selectionHandle == 0)
        {
          newOriginGuid = Guid.Empty;
          newOriginTag = String.Empty;
          newOriginBox = null;
          newOriginAnchor = -1;

          form1.ToolStripStatusLabel.Text = "";
        }

        if (selectionHandle == arrowBeingModified.ControlPoints.Count - 1)
        {
          newDestinationGuid = Guid.Empty;
          newDestinationTag = String.Empty;
          newDestinationBox = null;
          newDestinationAnchor = -1;

          form1.ToolStripStatusLabel.Text = "";
        }
      }

      fcFlowChart.RecreateCacheImage();
    }

    private void fcFlowChart_ArrowCreated(object sender, ArrowEventArgs e)
    {
      tempArrowKey++;
      String newLinkTag = "A_" + tempArrowKey.ToString(CultureInfo.InvariantCulture);
      e.Arrow.Text = newLinkTag;

      GraphicNode origin = null;
      GraphicNode destination = null;
      ModelNode modelOrigin = null;
      ModelNode modelDestination = null;
      String originPort = "";
      String destinationPort = "";
      Int16 originPortID = -1;
      Int16 destinationPortID = -1;
      List<SysCAD.Protocol.Point> controlPoints;
      SysCAD.Protocol.Rectangle textArea;

      if (newDestinationBox != null)
      {
        if (!((state.ClientProtocol.graphic.Nodes.TryGetValue((newDestinationBox.Tag as EditorNode).Guid, out destination)) &&
          (state.ClientProtocol.model.Nodes.TryGetValue(destination.ModelGuid, out modelDestination))))
        {
          state.ClientProtocol.LogMessage(out requestId, "Node missing for destination Box " + newDestinationBox.Tag, SysCAD.Log.MessageType.Error);
          return;
        }

        if (newDestinationAnchor != -1)
        {
          String fullAnchor = (newDestinationBox.Tag as EditorNode).anchorIntToTag[newDestinationAnchor];
          String anchorName = fullAnchor.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
          Int16 anchorID = Convert.ToInt16(fullAnchor.Substring(anchorName.Length), CultureInfo.InvariantCulture);
          destinationPort = anchorName;
          destinationPortID = anchorID;
        }
      }

      if (oldOriginBox != null)
      {
        if (!((state.ClientProtocol.graphic.Nodes.TryGetValue((newOriginBox.Tag as EditorNode).Guid, out origin)) &&
          (state.ClientProtocol.model.Nodes.TryGetValue(origin.ModelGuid, out modelOrigin))))
        {
          state.ClientProtocol.LogMessage(out requestId, "Node missing for origin Box " + newDestinationBox.Tag, SysCAD.Log.MessageType.Error);
          return;
        }

        if (oldOriginAnchor != -1)
        {
          String fullAnchor = (oldOriginBox.Tag as EditorNode).anchorIntToTag[oldOriginAnchor];
          String anchorName = fullAnchor.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
          Int16 anchorID = Convert.ToInt16(fullAnchor.Substring(anchorName.Length), CultureInfo.InvariantCulture);
          originPort = anchorName;
          originPortID = anchorID;
        }
      }

      controlPoints = State.GetControlPoints(e.Arrow.ControlPoints);

      textArea = new SysCAD.Protocol.Rectangle(0.0, 0.0, 0.0, 0.0);


      fcFlowChart.DeleteObject(e.Arrow);

      form1.ToolStripStatusLabel.Text = "";

      //arrowBeingModified.CustomDraw = CustomDraw.None;
      arrowBeingModifiedSelectionHandle = -1;
      arrowBeingModified = null;
      originAnchorChosen = null;
      destinationAnchorChosen = null;

      ModelLink modelLink = new ModelLink(new Guid(), string.Empty, "Pipe-1", modelOrigin.Guid, modelDestination.Guid, originPort, destinationPort);
      GraphicLink graphicLink = new GraphicLink(new Guid(), modelLink.Guid, string.Empty, origin.Guid, originPortID, destination.Guid, destinationPortID, controlPoints, 1.0, textArea, 0.0, new SysCAD.Protocol.Font(), true);

      SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();

      action.Create.Add(modelLink);
      action.Create.Add(graphicLink);

      AddAction(action);
    }

    private void fcFlowChart_ArrowCreating(object sender, AttachConfirmArgs e)
    {
      SysCAD.Protocol.Point originPos = new SysCAD.Protocol.Point(e.Arrow.ControlPoints[0]);
      Box originBox = fcFlowChart.GetBoxAt(originPos.ToPointF(), 2.0F);

      originBox = (originBox.Tag as EditorNode).ModelBox;

      if (originBox != null)
      {
        int closestI = 0;
        Double closestDistance = Double.MaxValue;

        for (int i = 0; i < originBox.AnchorPattern.Points.Count; i++)
        {

          if (originBox.AnchorPattern.Points[i].AllowOutgoing)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
              originBox.AnchorPattern.Points[i].X,
              originBox.AnchorPattern.Points[i].Y,
              originBox.RotationAngle);

            Double thisDistance = Distance(originPos, anchorPointPos);

            if (thisDistance < closestDistance)
            {
              closestDistance = thisDistance;
              closestI = i;
            }
          }
        }

        newOriginGuid = (originBox.Tag as EditorNode).Guid;
        newOriginTag = (originBox.Tag as EditorNode).Tag;
        newOriginBox = originBox;

        newOriginAnchor = closestI;
      }

      DoArrowModifyingOperations(e.Arrow, e.Arrow.ControlPoints.Count - 1);
      //arrowBeingModified = e.Arrow;
      //arrowBeingModified.CustomDraw = CustomDraw.Additional;
      //arrowBeingModified.ZTop();
      //fcFlowChart.RecreateCacheImage();
    }

    private void fcFlowChart_ArrowDeleting(object sender, ArrowConfirmArgs e)
    {
      e.Confirm = false;
      MessageBox.Show("Not Implemented");
    }

    private void fcFlowChart_ArrowModified(object sender, ArrowMouseArgs e)
    {
      {
        int i = 2;

        while ((oldControlPoints.Count > 3) && (i < oldControlPoints.Count - 1))
        {

          if (MergePoints(oldControlPoints[i - 1], oldControlPoints[i]))
          {
            oldControlPoints.RemoveAt(i - 1);
            oldControlPoints.RemoveAt(i - 1);

            // Deal with the adjacent points being slightly out of algnment.
            if (Math.Abs(oldControlPoints[i - 1].X - oldControlPoints[i - 2].X) < Math.Abs(oldControlPoints[i - 1].Y - oldControlPoints[i - 2].Y))
            {
              oldControlPoints[i - 1].X = oldControlPoints[i - 2].X;
            }
            else
            {
              oldControlPoints[i - 1].Y = oldControlPoints[i - 2].Y;
            }
            i = 2;
          }

          else
          {
            i++;
          }
        }
      }

      if (MergePoints(oldControlPoints[0], oldControlPoints[1]))
      {
        oldControlPoints.RemoveAt(0);

        if (e.Arrow.CascadeOrientation == MindFusion.FlowChartX.Orientation.Horizontal)
          e.Arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Vertical;

        else
          e.Arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Horizontal;
      }

      if (MergePoints(oldControlPoints[oldControlPoints.Count - 1], oldControlPoints[oldControlPoints.Count - 2]))
      {
        oldControlPoints.RemoveAt(oldControlPoints.Count - 1);
      }

      form1.ToolStripStatusLabel.Text = "";

      //arrowBeingModified.CustomDraw = CustomDraw.None;
      arrowBeingModifiedSelectionHandle = -1;
      arrowBeingModified = null;
      originAnchorChosen = null;
      destinationAnchorChosen = null;

      GraphicLink graphicLink = (e.Arrow.Tag as EditorLink).GraphicLink as GraphicLink;

      if (oldOriginBox != null)
      {
        (e.Arrow.Tag as EditorLink).GraphicLink.Origin = oldOriginGuid;
        e.Arrow.Origin = oldOriginBox;
        e.Arrow.OrgnAnchor = oldOriginAnchor;
      }

      else if (newOriginBox != null)
      {
        (e.Arrow.Tag as EditorLink).GraphicLink.Origin = newOriginGuid;
        e.Arrow.Origin = newOriginBox;
        e.Arrow.OrgnAnchor = newOriginAnchor;
      }

      if (oldDestinationBox != null)
      {
        (e.Arrow.Tag as EditorLink).GraphicLink.Destination = oldDestinationGuid;
        e.Arrow.Destination = oldDestinationBox;
        e.Arrow.DestAnchor = oldDestinationAnchor;
      }

      else if (newDestinationBox != null)
      {
        (e.Arrow.Tag as EditorLink).GraphicLink.Destination = newDestinationGuid;
        e.Arrow.Destination = newDestinationBox;
        e.Arrow.DestAnchor = newDestinationAnchor;
      }

      State.SetControlPoints(e.Arrow, oldControlPoints);

      EditorNode originItem = e.Arrow.Origin.Tag as EditorNode;
      EditorNode destinationItem = e.Arrow.Destination.Tag as EditorNode;

      //Guid originGuid = Guid.Empty;
      //Guid destinationGuid = Guid.Empty;

      //if (originItem != null)
      //  originGuid = originItem.Guid;

      //if (destinationItem != null)
      //  destinationGuid = destinationItem.Guid;

      String originFullAnchor = null;
      String destinationFullAnchor = null;
      String originAnchorName = null;
      String destinationAnchorName = null;

      originItem.anchorIntToTag.TryGetValue(e.Arrow.OrgnAnchor, out originFullAnchor);

      if (originFullAnchor != null)
      {
        originAnchorName = originFullAnchor.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        graphicLink.OriginPortID = Convert.ToInt16(originFullAnchor.Substring(originAnchorName.Length), CultureInfo.InvariantCulture);
      }

      destinationItem.anchorIntToTag.TryGetValue(e.Arrow.DestAnchor, out destinationFullAnchor);

      if (destinationFullAnchor != null)
      {
        destinationAnchorName = destinationFullAnchor.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        graphicLink.DestinationPortID = Convert.ToInt16(destinationFullAnchor.Substring(destinationAnchorName.Length), CultureInfo.InvariantCulture);
      }

      SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();
      if (graphicLink != null)
      {
        GraphicLink newGraphicLink = graphicLink.Clone();
        newGraphicLink.ControlPoints = State.GetControlPoints(e.Arrow.ControlPoints);
        action.Modify.Add(newGraphicLink);
      }


      AddAction(action);

      //if (!state.ModifyGraphicLink(out requestId,
      //  graphicLink.Guid,
      //  graphicLink.Tag,
      //  originGuid,
      //  destinationGuid,
      //  originAnchorName,
      //  originAnchorID,
      //  destinationAnchorName,
      //  destinationAnchorID,
      //  oldControlPoints,
      //  graphicLink.TagArea,
      //  graphicLink.TagAngle))
      //{ // failure, revert back to previous.
      //  // do something here...
      //}

      oldOriginGuid = Guid.Empty;
      oldOriginBox = null;
      oldOriginAnchor = -1;

      oldDestinationGuid = Guid.Empty;
      oldDestinationBox = null;
      oldDestinationAnchor = -1;

      newOriginGuid = Guid.Empty;
      newOriginTag = String.Empty;
      newOriginBox = null;
      newOriginAnchor = -1;

      newDestinationGuid = Guid.Empty;
      newDestinationTag = String.Empty;
      newDestinationBox = null;
      newDestinationAnchor = -1;
    }

    private void fcFlowChart_ArrowModifying(object sender, ArrowConfirmArgs e)
    {

      form1.ModeModify();

      if (fcFlowChart.Selection.Arrows.Count == 1) // We're playing with just one arrow...
      {
        DoArrowModifyingOperations(e.Arrow, e.SelectionHandle);
      }
    }

    private void fcFlowChart_BoxDeleting(object sender, BoxConfirmArgs e)
    {
      e.Confirm = false; // Tell flowchart not to deal with this.
      DeleteSelection();
    }

    private void fcFlowChart_Click(object sender, EventArgs e)
    {
      MouseEventArgs me = e as MouseEventArgs;
      SetHoverMembers(me);

      form1.ModeModify();

      if (me.Button == MouseButtons.Right)
      {
        ContextMenu theMenu = new ContextMenu();

        if (hoverNode != null)
        {
          contextNode = hoverNode;
          if (hoverNode.Locked)
            theMenu.MenuItems.Add("Unlock", new EventHandler(UnlockNode));
          else
          {
            MenuItem arrangeMenu = theMenu.MenuItems.Add("Align Nodes...");
            arrangeMenu.MenuItems.Add("Top", new EventHandler(AlignTop));
            arrangeMenu.MenuItems.Add("Bottom", new EventHandler(RouteLinks));
            arrangeMenu.MenuItems.Add("Left", new EventHandler(RouteLinks));
            arrangeMenu.MenuItems.Add("Right", new EventHandler(RouteLinks));
            arrangeMenu.MenuItems.Add("Center (H)", new EventHandler(RouteLinks));
            arrangeMenu.MenuItems.Add("Center (V)", new EventHandler(RouteLinks));
            theMenu.MenuItems.Add("-");
            theMenu.MenuItems.Add("Route Links", new EventHandler(RouteLinks));
            theMenu.MenuItems.Add("Raise to Top", new EventHandler(RaiseNodeToTop));
            theMenu.MenuItems.Add("Send to Bottom", new EventHandler(SendNodeToBottom));

            theMenu.MenuItems.Add("-");

            theMenu.MenuItems.Add("Lock", new EventHandler(LockNode));
          }
          form1.ModeModify();
        }

        else if (hoverArea != null)
        {
          contextArea = hoverArea;
          if (hoverArea.Locked)
            theMenu.MenuItems.Add("Unlock", new EventHandler(UnlockArea));
          else
          {
            theMenu.MenuItems.Add("Raise to Top", new EventHandler(RaiseAreaToTop));
            theMenu.MenuItems.Add("Send to Bottom", new EventHandler(SendAreaToBottom));

            theMenu.MenuItems.Add("-");

            theMenu.MenuItems.Add("Lock", new EventHandler(LockArea));
          }
          form1.ModeModify();
        }

        else if (hoverLink != null)
        {
          theMenu.MenuItems.Add("Route Link", new EventHandler(RouteLink));
          theMenu.MenuItems.Add("Disconnect Origin", new EventHandler(DisconnectOrigin));
          theMenu.MenuItems.Add("Disconnect Destination", new EventHandler(DisconnectDestination));

          form1.ModeModify();
        }

        theMenu.MenuItems.Add("-");
        theMenu.MenuItems.Add("Layout Flowchart (!!Broken in an interesting way in latest release!!)", new EventHandler(LayoutFlowchart));

        theMenu.MenuItems.Add("Insert Symbol", new EventHandler(InsertSymbol));

        theMenu.Show(fcFlowChart, me.Location);
      }
    }

    private void fcFlowChart_DrawArrow(object sender, ArrowDrawArgs e)
    {
      Arrow arrow = e.Arrow;
      EditorLink link = arrow.Tag as EditorLink;

      if (arrow != arrowBeingModified)
      {

        if (arrowBeingModified == null)
        {
          {
            MindFusion.FlowChartX.Node node = arrow.Destination;
            Box box = null;

            if (node != null)
              box = node as Box;

            if ((box != null) && (arrow.DestAnchor != -1))
            {
              PointF anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(box.BoundingRect),
                box.AnchorPattern.Points[arrow.DestAnchor].X,
                box.AnchorPattern.Points[arrow.DestAnchor].Y,
                box.RotationAngle).ToPointF();

              PointF[] extensionPoints =
                new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos };

              System.Drawing.Pen pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, 0, 0, 255), 0.0F);
              e.Graphics.DrawLines(pen, extensionPoints);

              pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, Color.Green), 0.0F);
              SysCAD.Protocol.Rectangle anchorPointRect = new SysCAD.Protocol.Rectangle(anchorPointPos, SysCAD.Protocol.Size.Empty);
              anchorPointRect.Inflate((double)fcFlowChart.SelHandleSize, (double)fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, anchorPointRect);
            }

            else
            {
              Color errorColor = new Color();

              if (box != null)
              {
                errorColor = Color.Yellow;
                PointF anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(box.BoundingRect),
                  50,
                  50,
                  box.RotationAngle).ToPointF();

                PointF[] extensionPoints =
                  new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos };

                System.Drawing.Pen pen1 = new System.Drawing.Pen(Color.FromArgb(link.Opacity, errorColor), 0.0F);
                e.Graphics.DrawLines(pen1, extensionPoints);
              }
              else
                errorColor = Color.Red;

              System.Drawing.Pen pen = new System.Drawing.Pen(errorColor, fcFlowChart.SelHandleSize);
              PointF controlPoint = arrow.ControlPoints[arrow.ControlPoints.Count - 1];
              SysCAD.Protocol.Rectangle controlPointRect = new SysCAD.Protocol.Rectangle(controlPoint, SysCAD.Protocol.Size.Empty);
              controlPointRect.Inflate(fcFlowChart.SelHandleSize, fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, controlPointRect);
            }
          }

          {
            MindFusion.FlowChartX.Node node = arrow.Origin;
            Box box = null;

            if (node != null)
              box = node as Box;

            if ((box != null) && (arrow.OrgnAnchor != -1))
            {
              PointF anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(box.BoundingRect),
                box.AnchorPattern.Points[arrow.OrgnAnchor].X,
                box.AnchorPattern.Points[arrow.OrgnAnchor].Y,
                box.RotationAngle).ToPointF();

              PointF[] extensionPoints =
                new PointF[] { arrow.ControlPoints[0], anchorPointPos };

              System.Drawing.Pen pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, 0, 0, 255), 0.0F);
              e.Graphics.DrawLines(pen, extensionPoints);

              pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, Color.Green), 0.0F);
              SysCAD.Protocol.Rectangle r = new SysCAD.Protocol.Rectangle(anchorPointPos, SysCAD.Protocol.Size.Empty);
              r.Inflate(fcFlowChart.SelHandleSize, fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, r);
            }

            else
            {
              Color errorColor = new Color();

              if (box != null)
              {
                errorColor = Color.Yellow;

                PointF anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(box.BoundingRect),
                  50,
                  50,
                  box.RotationAngle).ToPointF();

                PointF[] extensionPoints =
                  new PointF[] { arrow.ControlPoints[0], anchorPointPos };

                System.Drawing.Pen pen1 = new System.Drawing.Pen(Color.FromArgb(link.Opacity, Color.Yellow), 0.0F);
                e.Graphics.DrawLines(pen1, extensionPoints);
              }
              else
                errorColor = Color.Red;

              System.Drawing.Pen pen = new System.Drawing.Pen(errorColor, fcFlowChart.SelHandleSize);
              PointF p = arrow.ControlPoints[0];
              SysCAD.Protocol.Rectangle r = new SysCAD.Protocol.Rectangle(p, SysCAD.Protocol.Size.Empty);
              r.Inflate(fcFlowChart.SelHandleSize, fcFlowChart.SelHandleSize);
              e.Graphics.DrawEllipse(pen, r);
            }
          }
        }
      }

      else // this is an arrowbeingmodified...
      {
        {

          if (newDestinationAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(newDestinationBox.BoundingRect),
              newDestinationBox.AnchorPattern.Points[newDestinationAnchor].X,
              newDestinationBox.AnchorPattern.Points[newDestinationAnchor].Y,
              newDestinationBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, Color.LightBlue), 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }

          else if (oldDestinationAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(oldDestinationBox.BoundingRect),
              oldDestinationBox.AnchorPattern.Points[oldDestinationAnchor].X,
              oldDestinationBox.AnchorPattern.Points[oldDestinationAnchor].Y,
              oldDestinationBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[arrow.ControlPoints.Count - 1], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, 0, 0, 255), 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
        }

        {

          if (newOriginAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(newOriginBox.BoundingRect),
              newOriginBox.AnchorPattern.Points[newOriginAnchor].X,
              newOriginBox.AnchorPattern.Points[newOriginAnchor].Y,
              newOriginBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, Color.LightBlue), 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }

          else if (oldOriginAnchor != -1)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(oldOriginBox.BoundingRect),
              oldOriginBox.AnchorPattern.Points[oldOriginAnchor].X,
              oldOriginBox.AnchorPattern.Points[oldOriginAnchor].Y,
              oldOriginBox.RotationAngle);

            PointF[] extensionPoints =
              new PointF[] { arrow.ControlPoints[0], anchorPointPos.ToPointF() };

            System.Drawing.Pen pen = new System.Drawing.Pen(Color.FromArgb(link.Opacity, 0, 0, 255), 0.0F);

            e.Graphics.DrawLines(pen, extensionPoints);
          }
        }
      }
    }

    private void fcFlowChart_DrawBox(object sender, BoxDrawArgs e)
    {
      if (arrowBeingModified != null)
      {
        SysCAD.Protocol.Point originPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[0]);
        Box originBox = fcFlowChart.GetBoxAt(originPos.ToPointF(), 2.0F);

        if (originBox != null)
        {
          originBox = (originBox.Tag as EditorNode).ModelBox;

          if (originBox != null)
          {

            if (originBox.AnchorPattern != null)
            {
              //if (originAnchorChosen == null)
              {
                Double closest = Double.MaxValue;

                foreach (AnchorPoint anchorPoint in originBox.AnchorPattern.Points)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
                    anchorPoint.X,
                    anchorPoint.Y,
                    originBox.RotationAngle);

                  Double distance = Distance(anchorPointPos, originPos);

                  if (distance < closest)
                  {
                    closest = distance;
                    originAnchorChosen = anchorPoint.Tag as Anchor;
                  }
                }

                // if (state.PortCheck(out requestId, (originBox.Tag as Item).Guid, originAnchorChosen) == PortStatus.Available)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(originBox.BoundingRect),
                    (originAnchorChosen.Positions[0] as SysCAD.Protocol.Point).X,
                    (originAnchorChosen.Positions[0] as SysCAD.Protocol.Point).Y,
                    originBox.RotationAngle);

                  PointF[] extensionPoints =
                    new PointF[] { anchorPointPos.ToPointF(), anchorPointPos.ToPointF() };
                  System.Drawing.Pen pen = new System.Drawing.Pen(Color.Yellow, fcFlowChart.SelHandleSize);
                  e.Graphics.DrawEllipse(pen, RectangleF.FromLTRB(
                    anchorPointPos.ToPointF().X - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().X + fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y + fcFlowChart.SelHandleSize));
                  e.Graphics.DrawLines(pen, extensionPoints);
                }
              }
            }
          }
        }

        SysCAD.Protocol.Point destinationPos = new SysCAD.Protocol.Point(arrowBeingModified.ControlPoints[arrowBeingModified.ControlPoints.Count - 1]);
        Box destinationBox = fcFlowChart.GetBoxAt(destinationPos.ToPointF(), 2.0F);

        if (destinationBox != null)
        {
          destinationBox = (destinationBox.Tag as EditorNode).ModelBox;

          if (destinationBox != null)
          {

            if (destinationBox.AnchorPattern != null)
            {
              if (destinationAnchorChosen != null)
              {
                Double closest = Double.MaxValue;

                foreach (AnchorPoint anchorPoint in destinationBox.AnchorPattern.Points)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(destinationBox.BoundingRect),
                    anchorPoint.X,
                    anchorPoint.Y,
                    destinationBox.RotationAngle);

                  Double distance = Distance(anchorPointPos, destinationPos);

                  if (distance < closest)
                  {
                    closest = distance;
                    destinationAnchorChosen = anchorPoint.Tag as Anchor;
                  }
                }

                //if (state.PortCheck(out requestId, (destinationBox.Tag as Item).Guid, destinationAnchorChosen) == PortStatus.Available)
                {
                  SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(destinationBox.BoundingRect),
                    (destinationAnchorChosen.Positions[0] as SysCAD.Protocol.Point).X,
                    (destinationAnchorChosen.Positions[0] as SysCAD.Protocol.Point).Y,
                    destinationBox.RotationAngle);

                  PointF[] extensionPoints =
                    new PointF[] { anchorPointPos.ToPointF(), anchorPointPos.ToPointF() };
                  System.Drawing.Pen pen = new System.Drawing.Pen(Color.Yellow, fcFlowChart.SelHandleSize);
                  e.Graphics.DrawEllipse(pen, RectangleF.FromLTRB(
                    anchorPointPos.ToPointF().X - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y - fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().X + fcFlowChart.SelHandleSize,
                    anchorPointPos.ToPointF().Y + fcFlowChart.SelHandleSize));
                  e.Graphics.DrawLines(pen, extensionPoints);
                }

              }
            }
          }
        }
      }
    }

    private static void fcFlowChart_AreaDeleted(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private static void fcFlowChart_AreaModified(Int64 eventId, Int64 requestId, Guid guid, String tag, String path, SysCAD.Protocol.Rectangle boundingRect)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private static void fcFlowChart_ItemCreated(Int64 eventId, Int64 requestId, Guid guid)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private void fcFlowChart_PortInfoRequested(Int64 eventId, Int64 requestId, Guid guid, String tag, PortInfo portInfo)
    {
      throw new NotImplementedException("The method or operation is not implemented.");
    }

    private void fcFlowChart_LinkChanged(Int64 eventId, Int64 requestId, Guid guid, String tag, String classId, Guid origin, Guid destination, String originPort, String destinationPort, List<SysCAD.Protocol.Point> controlPoints, SysCAD.Protocol.Rectangle textArea, Double textAngle)
    {
      EditorLink link = state.Link(guid);

      if (link != null)
      {
        GraphicLink graphicLink = link.GraphicLink;
        ModelLink modelLink = link.ModelLink;
        graphicLink.Origin = origin;
        graphicLink.Destination = destination;
        modelLink.OriginPort = originPort;
        modelLink.DestinationPort = destinationPort;
        graphicLink.ControlPoints = controlPoints;

        graphicLink.TagArea = textArea;
        graphicLink.TagAngle = textAngle;

        Arrow arrow = link.Arrow;
        EditorNode originItem = null;
        EditorNode destinationItem = null;

        if (graphicLink.Origin != Guid.Empty) originItem = state.Item(graphicLink.Origin);

        if (graphicLink.Destination != Guid.Empty) destinationItem = state.Item(graphicLink.Destination);

        if (originItem != null)
          arrow.Origin = originItem.ModelBox;

        if (destinationItem != null)
          arrow.Destination = destinationItem.ModelBox;

        if ((modelLink.OriginPort != null) && ((originItem.ModelBox.Tag as EditorNode).anchorTagToInt.ContainsKey(modelLink.OriginPort + graphicLink.OriginPortID.ToString(CultureInfo.InvariantCulture))))
          arrow.OrgnAnchor = (originItem.ModelBox.Tag as EditorNode).anchorTagToInt[modelLink.OriginPort + graphicLink.OriginPortID.ToString(CultureInfo.InvariantCulture)];

        else
          arrow.OrgnAnchor = -1;

        if ((modelLink.DestinationPort != null) && ((destinationItem.ModelBox.Tag as EditorNode).anchorTagToInt.ContainsKey(modelLink.DestinationPort + graphicLink.DestinationPortID.ToString(CultureInfo.InvariantCulture))))
          arrow.DestAnchor = (destinationItem.ModelBox.Tag as EditorNode).anchorTagToInt[modelLink.DestinationPort + graphicLink.DestinationPortID.ToString(CultureInfo.InvariantCulture)];

        else
          arrow.DestAnchor = -1;

        String toolTipOriginPort = modelLink.OriginPort;
        String toolTipDestinationPort = modelLink.DestinationPort;

        if (toolTipOriginPort == null) toolTipOriginPort = "*";

        if (toolTipDestinationPort == null) toolTipDestinationPort = "*";

        String originItemTag = "--";

        if (originItem != null)
          originItemTag = originItem.Tag;

        String destinationItemTag = "--";

        if (destinationItem != null)
          destinationItemTag = destinationItem.Tag;

        arrow.ToolTip = "Tag:" + graphicLink.Tag +
          "\nSrc: " + originItemTag + ":" + toolTipOriginPort +
          "\nDst: " + destinationItemTag + ":" + toolTipDestinationPort;
        arrow.ArrowHead = ArrowHead.Triangle;
        arrow.Style = ArrowStyle.Cascading;

        if (graphicLink.ControlPoints != null && graphicLink.ControlPoints.Count > 1)
        {
          State.SetControlPoints(arrow, graphicLink.ControlPoints);
        }
      }
    }

    private void fcFlowChart_MouseMove(object sender, MouseEventArgs e)
    {
      SetHoverMembers(e);

      EditorNode editorNode = null;
      form1.fcHoverview.ClearAll();
      form1.hoverviewIncomingLinks.Clear();
      form1.hoverviewOutgoingLinks.Clear();
      // Update Hoverview.

      if (hoverNode != null)
      {
        editorNode = hoverNode;
        form1.hoverviewIncomingLinks.Clear();
        form1.hoverviewOutgoingLinks.Clear();
        if (hoverNode.ModelBox != null)
        {
          foreach (Arrow arrow in hoverNode.ModelBox.IncomingArrows)
            form1.hoverviewIncomingLinks.Add(arrow.Tag as EditorLink);
          foreach (Arrow arrow in hoverNode.ModelBox.OutgoingArrows)
            form1.hoverviewOutgoingLinks.Add(arrow.Tag as EditorLink);
        }
      }
      else if (fcFlowChart.Selection.Boxes.Count == 1)
      {
        editorNode = fcFlowChart.Selection.Boxes[0].Tag as EditorNode;
        form1.hoverviewIncomingLinks.Clear();
        form1.hoverviewOutgoingLinks.Clear();
        foreach (Arrow arrow in fcFlowChart.Selection.Boxes[0].IncomingArrows)
          form1.hoverviewIncomingLinks.Add(arrow.Tag as EditorLink);
        foreach (Arrow arrow in fcFlowChart.Selection.Boxes[0].OutgoingArrows)
          form1.hoverviewOutgoingLinks.Add(arrow.Tag as EditorLink);

      }

      if (editorNode != null)
      {
        ModelNode modelNode = editorNode.ModelNode;
        if (modelNode != null)
        {
          ModelStencil modelStencil = State.ModelStencil(modelNode.NodeClass);

          if (modelStencil != null)
          {
            float scale;
            if (form1.fcHoverview.Size.Width / modelStencil.DefaultSize.Width > form1.fcHoverview.Size.Height / modelStencil.DefaultSize.Height)
            {
              scale = (float)(form1.fcHoverview.Size.Height / modelStencil.DefaultSize.Height) / 5.0F;
            }
            else
            {
              scale = (float)(form1.fcHoverview.Size.Width / modelStencil.DefaultSize.Width) / 5.0F;
            }

            //float scale = 100.0F / (float)Math.Sqrt((float)graphicStencil.defaultSize.Width * (float)graphicStencil.default5Size.Width + (float)graphicStencil.defaultSize.Height * (float)graphicStencil.defaultSize.Height);
            form1.fcHoverview.AntiAlias = SmoothingMode.AntiAlias;


            form1.hoverviewBox = form1.fcHoverview.CreateBox(0.0F, 0.0F, (float)modelStencil.DefaultSize.Width * scale, (float)modelStencil.DefaultSize.Height * scale);
            form1.hoverviewBox.FillColor = System.Drawing.Color.FromArgb(220, 222, 184, 136);
            form1.hoverviewBox.FrameColor = System.Drawing.Color.FromArgb(255, 111, 92, 68);

            //Table table = form1.fcHoverview.CreateTable(0.0F, 10.0F + (float)graphicStencil.defaultSize.Height * scale, 80.0F, 30.0F);
            //table.Caption = "Ports";
            //table.ColumnCount = 2;
            //int i = table.AddRow();

            if (modelStencil != null)
              form1.hoverviewBox.Shape = State.GetShapeTemplate(modelStencil, false, false);
            else
              form1.hoverviewBox.Shape = ShapeTemplate.FromId("Decision2");

            form1.hoverviewBox.AnchorPattern = State.GetAnchorPattern(modelStencil, editorNode);

            form1.hoverviewBox.Tag = editorNode;
            form1.hoverviewBox.Visible = true;
            form1.hoverviewBox.Style = BoxStyle.Shape;

            foreach (EditorLink link in form1.hoverviewIncomingLinks)
            {
              Arrow arrow = state.Arrow(link.Guid);

              int portId = 0;
              (editorNode.ModelBox.Tag as EditorNode).anchorTagToInt.TryGetValue(link.ModelLink.DestinationPort + link.GraphicLink.DestinationPortID.ToString(CultureInfo.InvariantCulture), out portId);

              if (form1.hoverviewBox.AnchorPattern.Points.Count > portId)
              {
                PointF anchorPointPos = FrmFlowChart.GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(form1.hoverviewBox.BoundingRect),
                  form1.hoverviewBox.AnchorPattern.Points[portId].X,
                  form1.hoverviewBox.AnchorPattern.Points[portId].Y,
                  form1.hoverviewBox.RotationAngle).ToPointF();

                PointF pt1 = Flowchart2Hoverbox(link.Arrow.Destination.BoundingRect,
                                   form1.hoverviewBox.BoundingRect,
                                   arrow.ControlPoints[arrow.ControlPoints.Count - 2]);
                PointF pt2 = Flowchart2Hoverbox(link.Arrow.Destination.BoundingRect,
                                   form1.hoverviewBox.BoundingRect,
                                   arrow.ControlPoints[arrow.ControlPoints.Count - 1]);

                ///Box box1 = form1.fcHoverview.CreateBox(pt1.X*2.0F, pt1.Y, 0.001F, 0.01F);
                //box1.Visible = false;
                //box1.FillColor = System.Drawing.Color.FromArgb(20, 222, 184, 136);
                //box1.FrameColor = System.Drawing.Color.FromArgb(25, 111, 92, 68);
                //Box box2 = form1.fcHoverview.CreateBox(pt2.X, pt2.Y, 0.001F, 0.01F);
                //box2.Visible = false;
                Arrow arrow1 = form1.fcHoverview.CreateArrow(pt1, pt2);
                Arrow arrow2 = form1.fcHoverview.CreateArrow(pt2, anchorPointPos);
                arrow1.ArrowHead = ArrowHead.Triangle;
                arrow1.ArrowHeadSize = 2.0F;
                arrow1.FillColor = Color.Black;

                arrow2.PenColor = Color.Blue;
              }


              form1.fcHoverview.Selection.Clear();
            }

            foreach (EditorLink link in form1.hoverviewOutgoingLinks)
            {
              Arrow arrow = state.Arrow(link.Guid);

              int portId = 0;
              (editorNode.ModelBox.Tag as EditorNode).anchorTagToInt.TryGetValue(link.ModelLink.OriginPort + link.GraphicLink.OriginPortID.ToString(CultureInfo.InvariantCulture), out portId);

              if (form1.hoverviewBox.AnchorPattern.Points.Count > portId)
              {
                PointF anchorPointPos = FrmFlowChart.GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(form1.hoverviewBox.BoundingRect),
                                form1.hoverviewBox.AnchorPattern.Points[portId].X,
                                form1.hoverviewBox.AnchorPattern.Points[portId].Y,
                                form1.hoverviewBox.RotationAngle).ToPointF();

                PointF pt1 = Flowchart2Hoverbox(link.Arrow.Origin.BoundingRect,
                                   form1.hoverviewBox.BoundingRect,
                                   arrow.ControlPoints[1]);
                PointF pt2 = Flowchart2Hoverbox(link.Arrow.Origin.BoundingRect,
                                   form1.hoverviewBox.BoundingRect,
                                   arrow.ControlPoints[0]);

                ///Box box1 = form1.fcHoverview.CreateBox(pt1.X*2.0F, pt1.Y, 0.001F, 0.01F);
                //box1.Visible = false;
                //box1.FillColor = System.Drawing.Color.FromArgb(20, 222, 184, 136);
                //box1.FrameColor = System.Drawing.Color.FromArgb(25, 111, 92, 68);
                //Box box2 = form1.fcHoverview.CreateBox(pt2.X, pt2.Y, 0.001F, 0.01F);
                //box2.Visible = false;
                Arrow arrow1 = form1.fcHoverview.CreateArrow(pt2, pt1);
                Arrow arrow2 = form1.fcHoverview.CreateArrow(pt2, anchorPointPos);
                arrow1.ArrowHead = ArrowHead.Triangle;
                arrow1.ArrowHeadSize = 2.0F;
                arrow1.FillColor = Color.Black;

                arrow2.PenColor = Color.Blue;
              }
              form1.fcHoverview.Selection.Clear();
            }

            RectangleF zoomRect = form1.hoverviewBox.BoundingRect;
            zoomRect.Inflate(form1.hoverviewBox.BoundingRect.Width * 0.4F, form1.hoverviewBox.BoundingRect.Height * 0.4F);
            form1.fcHoverview.ZoomToRect(zoomRect);
          }
        }
      }

      if ((hoverNode != null) && (hoverNode.ModelBox != null) && (hoverNode.ModelBox.AnchorPattern != null))
      {
        int closestI = 0;
        Double closestDistance = Double.MaxValue;

        for (int i = 0; i < hoverNode.ModelBox.AnchorPattern.Points.Count; i++)
        {

          if (hoverNode.ModelBox.AnchorPattern.Points[i].AllowOutgoing)
          {
            SysCAD.Protocol.Point anchorPointPos = GetRelativeAnchorPosition(new SysCAD.Protocol.Rectangle(hoverNode.ModelBox.BoundingRect),
              hoverNode.ModelBox.AnchorPattern.Points[i].X,
              hoverNode.ModelBox.AnchorPattern.Points[i].Y,
              hoverNode.ModelBox.RotationAngle);
            Double thisDistance = Distance(new SysCAD.Protocol.Point(fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y))), anchorPointPos);

            if (thisDistance < closestDistance)
            {
              closestDistance = thisDistance;
              closestI = i;
            }
          }
        }

        String hoverTag = hoverNode.Tag;
        int hoverAnchor = closestI;

        String anchorString;
        hoverNode.anchorIntToTag.TryGetValue(hoverAnchor, out anchorString);

        if (anchorString != null)
        {
          form1.ToolStripStatusLabel.Text = "Item: " + hoverTag +
  " : " + " Port: " + anchorString.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        }
      }

      if (oldHoverLink != null)
        if (oldHoverLink != hoverLink) // we've moved on, un-hover the old one.
          oldHoverLink.Hovered = false;

      if (oldHoverNode != null) // deal with old itemBox.
        if (oldHoverNode != hoverNode) // we've moved on, un-hover the old one.
          oldHoverNode.Hovered = false;

      if (hoverLink != null)
        hoverLink.Hovered = true;

      if (hoverNode != null)
        hoverNode.Hovered = true;

      oldHoverLink = hoverLink;
      oldHoverNode = hoverNode;
    }

    private void SetHoverMembers(MouseEventArgs e)
    {
      PointF ptF = fcFlowChart.ClientToDoc(new System.Drawing.Point(e.X, e.Y));

      Arrow hoverArrow = fcFlowChart.GetArrowAt(ptF, 2.0F);
      Box hoverBox = fcFlowChart.GetBoxAt(ptF, 2.0F);

      hoverLink = null;
      hoverNode = null;
      hoverArea = null;

      if ((hoverArrow != null) && (hoverArrow.Tag is EditorLink))
        hoverLink = hoverArrow.Tag as EditorLink;

      if ((hoverBox != null) && (hoverBox.Tag is EditorNode))
        hoverNode = hoverBox.Tag as EditorNode;
      else
      {
        foreach (EditorNode node in state.Nodes)
        {
          if (node.Locked)
          {
            if (((node.ModelBox != null) && (node.ModelBox.BoundingRect.Contains(ptF))) ||
              ((node.GraphicBox != null) && (node.GraphicBox.BoundingRect.Contains(ptF))) ||
              ((node.TextBox != null) && (node.TextBox.BoundingRect.Contains(ptF))) ||
              ((node.HiddenBox != null) && (node.HiddenBox.BoundingRect.Contains(ptF))))
            {
              hoverNode = node;
              return;
            }
          }
        }
      }

      if ((hoverBox != null) && (hoverBox.Tag is EditorArea))
        hoverArea = (hoverBox.Tag as EditorArea);
      else
      {
        foreach (EditorArea area in state.Areas)
        {
          if (area.Box.BoundingRect.Contains(ptF))
          {
            hoverArea = area;
            return;
          }
        }
      }
    }

    private PointF Flowchart2Hoverbox(RectangleF flowchartbb, RectangleF hoverboxbb, PointF pt)
    {
      return new PointF((pt.X - flowchartbb.X) / flowchartbb.Width * hoverboxbb.Width + hoverboxbb.X,
      (pt.Y - flowchartbb.Y) / flowchartbb.Height * hoverboxbb.Height + hoverboxbb.Y);
    }

    void fcFlowChart_MouseWheel(object sender, MouseEventArgs args)
    {

      if (arrowBeingModified != null)
      {

        if (newOriginAnchor != -1)
        {

          if (arrowBeingModifiedSelectionHandle == 0)
          {

            if (args.Delta > 0)
              newOriginAnchor++;

            else
              newOriginAnchor--;

            if (newOriginAnchor >= newOriginBox.AnchorPattern.Points.Count)
              newOriginAnchor = 0;

            if (newOriginAnchor < 0)
              newOriginAnchor = newOriginBox.AnchorPattern.Points.Count - 1;

            form1.ToolStripStatusLabel.Text =
              (newOriginBox.Tag as EditorNode).anchorIntToTag[newOriginAnchor];
          }
        }

        if (newDestinationAnchor != -1)
        {

          if (arrowBeingModifiedSelectionHandle > 0)
          {

            if (args.Delta > 0)
              newDestinationAnchor++;

            else
              newDestinationAnchor--;

            if (newDestinationAnchor >= newDestinationBox.AnchorPattern.Points.Count)
              newDestinationAnchor = 0;

            if (newDestinationAnchor < 0)
              newDestinationAnchor = newDestinationBox.AnchorPattern.Points.Count - 1;

            form1.ToolStripStatusLabel.Text =
              (newDestinationBox.Tag as EditorNode).anchorIntToTag[newDestinationAnchor];
          }
          refreshConnectedObjects();//arrowBeingModified);
        }
      }

      else
      {
        FlowChart fcSender = sender as FlowChart;

        float newScrollY = fcSender.ScrollY - args.Delta / 5;

        if (newScrollY > fcSender.DocExtents.Top)
          fcSender.ScrollY = newScrollY;
      }
    }

    private void fcFlowChart_PermissionsChanged(Int64 eventId, Int64 requestId, ClientBaseProtocol.Permissions permissions)
    {
      state.PermissionsChanged(permissions);

      form1.PermissionsChanged(permissions);
    }

    private void fcFlowChart_Step(Int64 eventId, Int64 step, DateTime time)
    {
      state.Step(step, time);
    }

    private void fcFlowChart_Sync(Int64 eventId)
    {
      form1.LoadProject(state.ClientProtocol, state.Config);
    }

    private void fcFlowChart_Changed(Int64 eventId, Int64 requestId, Actioned actioned)
    {
      DoCreated(actioned.Created);
      DoModified(actioned.Modified);
      DoDeleted(actioned.Deleted);
    }

    private void DoCreated(Collection<Guid> created)
    {
      if (created != null)
      {
        foreach (Guid guid in created)
        {
          GraphicNode graphicNode = null;
          GraphicLink graphicLink = null;

          if (state.ClientProtocol.graphic.Nodes.TryGetValue(guid, out graphicNode))
          {
            ModelNode modelNode;
            if (state.ClientProtocol.model.Nodes.TryGetValue(graphicNode.ModelGuid, out modelNode))
            {
              state.CreateNode(modelNode, graphicNode);
            }
            else
            {
              state.CreateNode(null, graphicNode);
            }
          }

          if (state.ClientProtocol.graphic.Links.TryGetValue(guid, out graphicLink))
          {
            ModelLink modelLink;
            if (state.ClientProtocol.model.Links.TryGetValue(graphicLink.ModelGuid, out modelLink))
            {
              state.CreateLink(modelLink, graphicLink);
            }
            else
            {
              state.ClientProtocol.LogMessage(out requestId, "Create: ModelLink missing for GraphcLink (Tag: " + graphicLink.Tag + ", Guid: " + graphicLink.Guid + ")", SysCAD.Log.MessageType.Error);
            }
          }
        }
      }
    }

    private void DoModified(Collection<Guid> modified)
    {
      if (modified != null)
      {
        foreach (Guid guid in modified)
        {
          state.ModifyNode(guid);
        }

        foreach (Guid guid in modified)
        {
          state.ModifyLink(guid);
        }
      }
    }

    private void DoDeleted(Collection<Guid> deleted)
    {
      if (deleted != null)
      {
        foreach (Guid guid in deleted)
        {
          state.DeleteNode(guid);
        }

        foreach (Guid guid in deleted)
        {
          state.DeleteLink(guid);
        }
      }
    }

    private void fcFlowChart_BoxModified(object sender, BoxMouseArgs e)
    {
      SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();

      if (e.Box.Tag is EditorNode)
      {
        Box modelBox = (e.Box.Tag as EditorNode).ModelBox;
        Box graphicBox = (e.Box.Tag as EditorNode).GraphicBox;
        Box textBox = (e.Box.Tag as EditorNode).TextBox;


        GraphicNode newGraphicNode = state.GraphicItem((e.Box.Tag as EditorNode).Guid).Clone();
        if (modelBox != null)
        {
          newGraphicNode.BoundingRect = new SysCAD.Protocol.Rectangle(modelBox.BoundingRect);
          newGraphicNode.Angle = modelBox.RotationAngle;

          if (textBox != e.Box)
          {
            ArrowCollection incomingArrows = modelBox.IncomingArrows.Clone();

            foreach (Arrow arrow in incomingArrows)
            {
              GraphicLink newGraphicLink = state.GraphicLink((arrow.Tag as EditorLink).Guid).Clone();

              if (newGraphicLink != null)
              {
                if (!State.CompareControlPoints(arrow.ControlPoints, newGraphicLink.ControlPoints))
                {
                  newGraphicLink.ControlPoints = State.GetControlPoints(arrow.ControlPoints);
                  action.Modify.Add(newGraphicLink);
                }
              }
            }

            ArrowCollection outgoingArrows = modelBox.OutgoingArrows.Clone();

            foreach (Arrow arrow in outgoingArrows)
            {
              GraphicLink newGraphicLink = state.GraphicLink((arrow.Tag as EditorLink).Guid).Clone();

              if (newGraphicLink != null)
              {
                if (!State.CompareControlPoints(arrow.ControlPoints, newGraphicLink.ControlPoints))
                {
                  newGraphicLink.ControlPoints = State.GetControlPoints(arrow.ControlPoints);
                  action.Modify.Add(newGraphicLink);
                }
              }
            }
          }
        }
        else
        {
          newGraphicNode.BoundingRect = new SysCAD.Protocol.Rectangle(graphicBox.BoundingRect);
          newGraphicNode.Angle = graphicBox.RotationAngle;
        }

        newGraphicNode.TagArea = new SysCAD.Protocol.Rectangle(textBox.BoundingRect);
        newGraphicNode.TagAngle = textBox.RotationAngle;
        action.Modify.Add(newGraphicNode);


        //Boxmodified.
        AddAction(action);
      }

      else if (e.Box.Tag is EditorLink)
      {
        Box tagArea = e.Box;
        GraphicLink newLink = state.GraphicLink((e.Box.Tag as EditorLink).Guid).Clone();

        newLink.TagArea = new SysCAD.Protocol.Rectangle(tagArea.BoundingRect);
        newLink.TagAngle = tagArea.RotationAngle;
        action.Modify.Add(newLink);

        AddAction(action);
      }

      else if (e.Box.Tag is EditorArea)
      { // move areas back to bottom after every operation so they stay out of the way.
        EditorArea editorArea = e.Box.Tag as EditorArea;
        editorArea.Box.ZBottom();
      }

      form1.GraphicPropertyGrid.Refresh();

      ContextMenu propertyGridMenu = form1.GraphicPropertyGrid.ContextMenu;

      if (propertyGridMenu == null)
        propertyGridMenu = new ContextMenu();

      propertyGridMenu.MenuItems.Add("Test");

      form1.GraphicPropertyGrid.ContextMenu = propertyGridMenu;
    }

    public void AddAction(SysCAD.Protocol.Action newAction)
    {
      actionExpire.Stop();
      foreach (Item item in newAction.Create)
      {
        action.Create.Add(item);
      }
      foreach (Item item in newAction.Modify)
      {
        action.Modify.Add(item);
      }
      actionExpire.Start();
    }

    private void DoChange(object sender, ElapsedEventArgs e)
    {
      actionExpire.Stop();
      SysCAD.Protocol.Action action2 = action;
      action = new SysCAD.Protocol.Action();
      if (!state.ClientProtocol.Change(out requestId, action2))
      { // Request was refused, undo manipulation...
        Collection<Guid> modified = new Collection<Guid>();

        foreach (Item item in action2.Modify)
        {
          modified.Add(item.Guid);
        }

        DoModified(modified);
      }

      form1.SetButtonStates();
    }

    private void fcFlowChart_BoxModifying(object sender, BoxConfirmArgs e)
    {
      //foreach (Arrow arrow in e.Box.IncomingArrows)
      //{
      //  if (arrow.ControlPoints.Count % 2 == 0)
      //  {
      //    // add dummy controlpoint to fix bug in mindfusion stuff.
      //    List<SysCAD.Protocol.Point> points = new List<SysCAD.Protocol.Point>();
      //    int i;
      //    for (i=0; i<arrow.ControlPoints.Count; i++)
      //    {
      //      points.Add(new SysCAD.Protocol.Point(arrow.ControlPoints[i].X, arrow.ControlPoints[i].Y));
      //    }
      //    points.Add(new SysCAD.Protocol.Point(arrow.ControlPoints[i].X, arrow.ControlPoints[i].Y)); // add dummy last one.

      //    State.SetControlPoints(arrow, points);
      //  }
      //}

      //foreach (Arrow arrow in e.Box.OutgoingArrows)
      //{
      //  if (arrow.ControlPoints.Count % 2 == 0)
      //  {
      //    // add dummy controlpoint to fix bug in mindfusion stuff.
      //    List<SysCAD.Protocol.Point> points = new List<SysCAD.Protocol.Point>();
      //    int i;
      //    for (i = 0; i < arrow.ControlPoints.Count; i++)
      //    {
      //      points.Add(new SysCAD.Protocol.Point(arrow.ControlPoints[i].X, arrow.ControlPoints[i].Y));
      //    }
      //    points.Add(new SysCAD.Protocol.Point(arrow.ControlPoints[i].X, arrow.ControlPoints[i].Y)); // add dummy last one.

      //    State.SetControlPoints(arrow, points);
      //  }
      //}

      if (e.Box.Tag is EditorNode)
      {
        Box modelBox = (e.Box.Tag as EditorNode).ModelBox;
        if (modelBox != null)
        {
          Box graphicBox = (e.Box.Tag as EditorNode).GraphicBox;
          //graphicBox.BoundingRect = (e.Box.Tag as Item).Model.BoundingRect;
          graphicBox.RotationAngle = (e.Box.Tag as EditorNode).ModelBox.RotationAngle;
        }
      }

      else if (e.Box.Tag is EditorArea)
      { // specifically modify the links so that they mvoe as expected.
        EditorArea editorArea = e.Box.Tag as EditorArea;

        double dx = (double)e.Box.BoundingRect.X - editorArea.GraphicArea.BoundingRect.X;
        double dy = (double)e.Box.BoundingRect.Y - editorArea.GraphicArea.BoundingRect.Y;

        PureComponents.TreeView.Node treeViewNode = state.TVNavigation.GetNodeByKey(editorArea.Guid.ToString());

        foreach (PureComponents.TreeView.Node subTreeViewNode in treeViewNode.Nodes)
        {
          if (subTreeViewNode.Tag is EditorNode)
          {
            EditorNode editorNode = subTreeViewNode.Tag as EditorNode;
            if (editorNode.ModelBox != null)
            {
              foreach (Arrow arrow in editorNode.IncomingArrows)
                State.SetControlPoints(arrow, (arrow.Tag as EditorLink).GraphicLink.ControlPoints, dx, dy);

              foreach (Arrow arrow in editorNode.OutgoingArrows)
                State.SetControlPoints(arrow, (arrow.Tag as EditorLink).GraphicLink.ControlPoints, dx, dy);
            }
          }
        }
      }
    }

    static public SysCAD.Protocol.Point GetRelativeAnchorPosition(SysCAD.Protocol.Rectangle nodeRect, Double x, Double y, Double angle)
    {
      SysCAD.Protocol.Point point = new SysCAD.Protocol.Point(nodeRect.X + nodeRect.Width * x / 100.0, nodeRect.Y + nodeRect.Height * y / 100.0);

      if (angle != 0)
      {
        point = rotatePointAt(point, getCenter(nodeRect), angle);
      }
      return point;
    }

    private void LayoutFlowchart(object sender, EventArgs e)
    {
      AnnealLayout layout = new AnnealLayout();
      layout.Arrange(fcFlowChart);
    }

    private void RaiseNodeToTop(object sender, EventArgs e)
    {
      hoverNode.GraphicBox.ZTop();
      hoverNode.ModelBox.ZTop();
      fcFlowChart.Invalidate();
    }

    private void RaiseAreaToTop(object sender, EventArgs e)
    {
      hoverArea.Box.ZTop();
      fcFlowChart.Invalidate();
    }

    private void refreshConnectedObjects() //Arrow arrowBeingModified)
    {
      fcFlowChart.Invalidate();
    }

    private void RouteLink(EditorLink link)
    {
      RouteLink(link.Arrow);
    }

    private void RouteLink(Arrow arrow)
    {
      arrow.Route();

      {
        List<SysCAD.Protocol.Point> controlPoints = State.GetControlPoints(arrow.ControlPoints);

        if (Math.Abs(controlPoints[0].X - controlPoints[1].X) <= fcFlowChart.MergeThreshold / 10.0)
          arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Auto;

        else if (Math.Abs(controlPoints[0].Y - controlPoints[1].Y) <= fcFlowChart.MergeThreshold / 10.0)
          arrow.CascadeOrientation = MindFusion.FlowChartX.Orientation.Auto;

        //graphicLink graphicLink = (arrow.Tag as EditorLink).graphicLink as graphicLink;

        //EditorNode originItem = arrow.Origin.Tag as EditorNode;
        //EditorNode destinationItem = arrow.Destination.Tag as EditorNode;

        //String originPort = "";
        //Int16 originPortID = 0;
        //String destinationPort = "";
        //Int16 destinationPortID = 0;

        //if (arrow.OrgnAnchor != -1)
        //{
        //  String fullAnchor = originItem.anchorIntToTag[arrow.OrgnAnchor];
        //  //originPort = fullAnchor.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        //  originPortID = Convert.ToInt16(fullAnchor.Substring(originPort.Length), CultureInfo.InvariantCulture);
        //}

        //if (arrow.DestAnchor != -1)
        //{
        //  String fullAnchor = destinationItem.anchorIntToTag[arrow.DestAnchor];
        //  //destinationPort = fullAnchor.TrimEnd(new char[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        //  destinationPortID = Convert.ToInt16(fullAnchor.Substring(originPort.Length), CultureInfo.InvariantCulture);
        //}

        //if (!state.ModifyGraphicLink(out requestId,
        //  graphicLink.Guid,
        //  graphicLink.Tag,
        //  originItem.Guid,
        //  destinationItem.Guid,
        //  originPort,
        //  originPortID,
        //  destinationPort,
        //  destinationPortID,
        //  controlPoints,
        //  graphicLink.TagArea,
        //  graphicLink.TagAngle))
        //{ // failure, revert back to previous.
        //  // do something here...
        //}
      }

      fcFlowChart.Invalidate();
    }

    private void InsertSymbol(object sender, EventArgs e)
    {
      GraphicStencilChangeForm gscf = new GraphicStencilChangeForm(state.GraphicUnselectedThumbnails, state.GraphicSelectedThumbnails, state.ModelStencils, state.GraphicStencils, "");
      if (gscf.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        GraphicStencil graphicStencil = null;
        state.GraphicStencils.TryGetValue(gscf.Value, out graphicStencil);
        if (graphicStencil != null)
        {
          SysCAD.Protocol.Rectangle boundingRect = new SysCAD.Protocol.Rectangle(hoverPoint.X, hoverPoint.Y, 0.0, 0.0);
          boundingRect.Inflate(graphicStencil.DefaultSize.Width, graphicStencil.DefaultSize.Height);
          GraphicNode node = new GraphicNode("", hoverArea.GraphicArea.Path + hoverArea.GraphicArea.Tag, Guid.NewGuid(), gscf.Value, boundingRect, 0.0, graphicStencil.TagArea, 0.0, new SysCAD.Protocol.Font(), true, 1.0, Color.Beige, FillMode.Alternate, false, false);
          SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();
          action.Create.Add(node);
          AddAction(action);
        }
      }

    }

    private void RouteLink(object sender, EventArgs e)
    {
      RouteLink(hoverLink);
    }

    private void AlignTop(object sender, EventArgs e)
    {
      foreach (Box box in fcFlowChart.Selection.Boxes)
      {
        if (box.Tag is EditorNode)
        {
          EditorNode editorNode = box.Tag as EditorNode;
          GraphicNode graphicNode = editorNode.GraphicNode;
          SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();
        }
      }
    }

    private void UnlockArea(object sender, EventArgs e)
    {
      contextArea.Locked = false;
      contextArea.Box.ZBottom();
    }

    private void LockArea(object sender, EventArgs e)
    {
      contextArea.Locked = true;
      contextArea.Box.ZBottom();
    }

    private void UnlockNode(object sender, EventArgs e)
    {
      contextNode.Locked = false;
      contextNode.UpdateVisibility();
    }

    private void LockNode(object sender, EventArgs e)
    {
      contextNode.Locked = true;
      contextNode.UpdateVisibility();
    }

    private void RouteLinks(object sender, EventArgs e)
    {
      ArrowCollection incomingArrows = hoverNode.IncomingArrows.Clone();
      ArrowCollection outgoingArrows = hoverNode.OutgoingArrows.Clone();

      foreach (Arrow arrow in incomingArrows)
      {
        RouteLink(arrow);
      }

      foreach (Arrow arrow in outgoingArrows)
      {
        RouteLink(arrow);
      }
    }

    private void SendNodeToBottom(object sender, EventArgs e)
    {
      hoverNode.GraphicBox.ZBottom();
      if (hoverNode.ModelBox != null) hoverNode.ModelBox.ZBottom();
      if (hoverNode.HiddenBox != null) hoverNode.HiddenBox.ZBottom();
      fcFlowChart.Invalidate();
    }

    private void SendAreaToBottom(object sender, EventArgs e)
    {
      hoverArea.Box.ZBottom();
      fcFlowChart.Invalidate();
    }

    public String CurrentModel
    {
      get { return currentModel; }
      set { currentModel = value; }
    }

    public String CurrentStencil
    {
      get { return currentStencil; }
      set { currentStencil = value; }
    }

    public FlowChart FlowChart
    {
      get { return fcFlowChart; }
    }

    public State State
    {
      get { return state; }
    }

    internal void Undo()
    {
      fcFlowChart.Selection.Clear();
      state.ClientProtocol.Undo(out requestId);
    }

    internal void Redo()
    {
      fcFlowChart.Selection.Clear();
      state.ClientProtocol.Redo(out requestId);

    }

    internal void DeleteSelection()
    {
      List<EditorNode> items = new List<EditorNode>();
      List<EditorLink> links = new List<EditorLink>();

      foreach (Box box in fcFlowChart.Selection.Boxes)
      {
        EditorNode item = box.Tag as EditorNode;
        if (!items.Contains(item))
        {
          items.Add(item);
        }
      }

      foreach (Arrow arrow in fcFlowChart.Selection.Arrows)
      {
        EditorLink link = arrow.Tag as EditorLink;
        if (!links.Contains(link))
        {
          links.Add(link);
        }
      }

      foreach (EditorNode item in items)
      {
        //state.DeleteGraphicNode(out requestId, item.Guid);
      }

      foreach (EditorLink link in links)
      {
        //state.DeleteGraphicLink(out requestId, link.Guid);
      }
    }

    void fcFlowChart_DragEnter(object sender, System.Windows.Forms.DragEventArgs e)
    {
      if (e.Data.GetDataPresent(typeof(ModelStencil)))
      {
        e.Effect = DragDropEffects.Move;
      }
    }

    void fcFlowChart_DragOver(object sender, System.Windows.Forms.DragEventArgs e)
    {
      //e.Effect = DragDropEffects.Move;
    }


    private void fcFlowChart_DragDrop(object sender, DragEventArgs e)
    {
      if (e.Data.GetDataPresent(typeof(ModelStencil)))
      {
        ModelStencil modelStencil = e.Data.GetData(typeof(ModelStencil)) as ModelStencil;
        GraphicStencil graphicStencil;
        if (state.GraphicStencils.TryGetValue(modelStencil.Tag, out graphicStencil))
        {
          if (state.TVNavigation.SelectedNode != null)
          {
            tempTagExtension++;
            //SysCAD.Protocol.Rectangle boundingRect = new SysCAD.Protocol.Rectangle(fcFlowChart.ClientToDoc(fcFlowChart.PointToClient(new System.Drawing.Point(e.X, e.Y))), graphicStencil.defaultSize);
            //SysCAD.Protocol.Rectangle textArea = new SysCAD.Protocol.Rectangle(0.0, 0.0, 0.0, 0.0);


            throw new NotImplementedException("The method or operation is not implemented.");
            //bool a = state.CreateGraphicItem(out requestId, out guid, "Item " + tempTagExtension.ToString(), state.TVNavigation.SelectedNode.FullPath + state.TVNavigation.PathSeparator,
            //modelStencil.Tag, modelStencil.Tag, 
            //boundingRect, 0.0, textArea, 0.0, Color.Empty, graphicStencil.fillMode, false, false);
          }
        }
      }
    }

    public void PropertyGridLinkChanged(GraphicLink graphicLink)
    {
      SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();
      action.Modify.Add(graphicLink);
      AddAction(action);
    }

    public void PropertyGridNodeChanged(GraphicNode graphicNode)
    {
      SysCAD.Protocol.Action action = new SysCAD.Protocol.Action();
      action.Modify.Add(graphicNode);
      AddAction(action);
    }
  }
}
