// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Collections;
using System.ComponentModel;
using System.IO;
using MindFusion.FlowChartX.Visitors;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Represents a selection of diagram items.
	/// </summary>
	[DefaultProperty("Style")]
	public sealed class Selection : MindFusion.FlowChartX.ChartObject
	{
		internal Selection(FlowChart parent) : base(parent)
		{
			pen = new Pen(Color.Black);
			brush = null;

			style = SelectionStyle.SelectionHandles;
			constructed = true;

			selectedBoxes = new BoxCollection();
			selectedHosts = new ControlHostCollection();
			selectedTables = new TableCollection();
			selectedArrows = new ArrowCollection();

			selectedItems = new ChartObjectCollection();

			allowMultiSel = true;

			arrowsToMove = new ChartObjectCollection();

			frameColor = SystemColors.Highlight;
			fillColor = SystemColors.Highlight;

			includeItemsIfIntersect = true;
		}

		public override ItemType getType()
		{
			return ItemType.Selection;
		}

		internal override void freeResources()
		{
		}

		// ************ modification ************

		internal override void startModify(PointF org, int handle, InteractionState ist)
		{
			base.startModify(org, handle, ist);
			relatedInteraction = ist;

			ptLastTopLeft.X = rect.Left;
			ptLastTopLeft.Y = rect.Top;
			ptLastTopLeft = flowChart.AlignPointToGrid(ptLastTopLeft);

			if (handle == 8 && selectedItems.Count > 0)
			{
				foreach (ChartObject obj in selectedItems)
					if (!obj.getModifying() && obj.canModify(handle))
						obj.startModify(org, 0, ist);
			}

			// find arrows whose both ends are selected
			arrowsToMove.Clear();
			MethodCallVisitor visitor = new MethodCallVisitor(
				new VisitOperation(HV_TranslateArrow));
			visitHierarchy(visitor);
		}

		internal override void updateModify(PointF current, InteractionState ist)
		{
			base.updateModify(current, ist);

			PointF pt1 = flowChart.InteractionStartPoint;
			PointF pt2 = current;
			PointF ptTLA = new PointF(rcSaved.Left, rcSaved.Top);
			ptTLA.X += pt2.X - pt1.X;
			ptTLA.Y += pt2.Y - pt1.Y;
			ptTLA = flowChart.AlignPointToGrid(ptTLA);
	
			base.modifyDX = ptTLA.X - ptLastTopLeft.X;
			base.modifyDY = ptTLA.Y - ptLastTopLeft.Y;
			ptLastTopLeft = ptTLA;

			if (base.modifyHandle == 8)
			{
				// translate all points of arrows having both their ends selected
				foreach (Arrow arrow in arrowsToMove)
					arrow.translatePoints(base.modifyDX, base.modifyDY);

				// modify selected objects
				foreach (ChartObject obj in selectedItems)
					if (!ignoreItem(obj, ist) && obj.canModify(base.modifyHandle) &&
						!(obj is Arrow && arrowsToMove.Contains(obj)))
							obj.modifyTranslate(base.modifyDX, base.modifyDY, true);
				rect.Offset(base.modifyDX, base.modifyDY);
			}
		}

		internal override void completeModify(PointF end, InteractionState ist)
		{
			base.completeModify(end, ist);
			relatedInteraction = null;
			RectangleF rcBounding = new RectangleF(0, 0, 0, 0);

			if (modifyHandle == 8)
			{
				MethodCallVisitor visitor = new MethodCallVisitor(
					new VisitOperation(updateNSARect));
				visitHierarchy(visitor);

				foreach (ChartObject obj in selectedItems)
				{
					RectangleF rcObj = obj.getRotatedBounds();
					rcBounding = Utilities.unionNonEmptyRects(rcBounding, rcObj);
				}
				rect = rcBounding;

				// beware of arrow routing; obj.completeModify invokations
				// might route many arrows repetitively
				flowChart.DontRouteForAwhile = true;

				// call EndModify method of all selected objects
				foreach (ChartObject obj in selectedItems)
					if (obj.getModifying() && obj.canModify(modifyHandle))
						obj.completeModify();

				// now that would route an arrow only once
				flowChart.DontRouteForAwhile = false;
				flowChart.routeAllArrows(this);

				// fire SelectionMoved event, for that was exactly what happened
				flowChart.fireSelectionMoved();
			}
		}

		internal override void startCreate(PointF org)
		{
			RectangleF tempRect = rect;
			constructed = false;
			base.startCreate(org);
			rect = tempRect;
		}

		internal void cancelCreate()
		{
			constructed = true;
			recalcRect();
		}

		internal override void completeCreate(PointF end)
		{
			base.completeCreate(end);

			clear();

			ChartObjectCollection newSel = new ChartObjectCollection();
			flowChart.getIntersectingObjects(rect,
				newSel, allowMultiSel, includeItemsIfIntersect);
			foreach (ChartObject obj in newSel)
				addObjToSelection(obj);
			newSel.Clear();

			if (selectedItems.Count == 0)
			{
				if (flowChart.ActiveObject != null)
					flowChart.fireDeactivationEvent();
				flowChart.ActiveObject = null;
			}
			else if (flowChart.ActiveObject == null ||
				!flowChart.ActiveObject.Selected)
			{
				if (flowChart.ActiveObject != null)
					flowChart.fireDeactivationEvent();
				flowChart.ActiveObject = selectedItems[0];
				flowChart.fireActivationEvent();
			}

			flowChart.fireSelectionChanged();

			recalcRect();
		}

		internal override bool allowModify(PointF current)
		{
			current = flowChart.AlignPointToGrid(current);

			if (modifyHandle == 8)
			{
				bool allow = true;
				RestrictToDoc restr = flowChart.RestrObjsToDoc;

				if (restr != RestrictToDoc.NoRestriction)
				{
					foreach (ChartObject obj in selectedItems)
					{
						RectangleF rc = obj.getBoundingRect();
						allow = allow && !flowChart.rectRestrict(ref rc);
					}
				}

				return allow;
			}

			return false;
		}

		internal override void cancelModify(InteractionState ist)
		{
			base.cancelModify(ist);
			relatedInteraction = null;

			// call CancelModify method of all selected objects
			foreach (ChartObject obj in selectedItems)
				if (obj.getModifying())
					obj.cancelModify(ist);
		}

		// ************ drawing ************

		public override void Draw(Graphics g, bool shadow)
		{
			if (shadow) return;
			if (constructed && selectedItems.Count < 2)
			{
				flowChart.drawActiveSelHandles(g);
				return;
			}

			if (!constructed || style != SelectionStyle.SelectionHandles)
			{
				RectangleF rc = Utilities.normalizeRect(rect);

				if (style == SelectionStyle.SemiTransparent)
				{
					System.Drawing.Brush spbr =
						new System.Drawing.SolidBrush(
						Color.FromArgb(75, fillColor));
					g.FillRectangle(spbr, rc.X, rc.Y, rc.Width, rc.Height);
					spbr.Dispose();

					System.Drawing.Pen pen = new System.Drawing.Pen(
						Color.FromArgb(255, frameColor), 0);
					g.DrawRectangle(pen, rc.X, rc.Y, rc.Width, rc.Height);
					pen.Dispose();
				}
				else
				{
					try
					{
						// drawing with a dashed pen sometimes throws a generic exception
						// in GDIPLUS.DLL, so catch everything or everyone will blame us;
						System.Drawing.Pen pen = new System.Drawing.Pen(FrameColor, 0);
						pen.DashStyle = DashStyle.Dash;
						g.DrawRectangle(pen, rc.X, rc.Y, rc.Width, rc.Height);
						pen.Dispose();
					}
					catch (Exception)
					{
						// sometimes the exception says 'out of memory'
						GC.Collect();
					}
				}
			}

			bool drawHandles = true;
			if (!flowChart.ShowHandlesOnDrag)
			{
				if (flowChart.IsModifying)
					drawHandles = false;
			}

			if (drawHandles)
			{
				foreach (ChartObject obj in selectedItems)
					obj.drawSelHandles(g, flowChart.SelMnpColor);

				flowChart.drawActiveSelHandles(g);
			}
		}

		internal override void drawSelHandles(Graphics g, Color color)
		{
		}

		/// <summary>
		/// Color of the selection interior.
		/// </summary>
		[Description("Gets or sets the color used to paint the selection rectangle interior.")]
		public new Color FillColor
		{
			get
			{
				return fillColor;
			}
			set
			{
				if (!fillColor.Equals(value))
				{
					fillColor = value;
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private bool ShouldSerializeFillColor()
		{
			return !fillColor.Equals(SystemColors.Highlight);
		}

		/// <summary>
		/// Color of the table borders.
		/// </summary>
		[Description("Gets or sets the color used to paint the selection rectangle frame lines.")]
		public new Color FrameColor
		{
			get
			{
				return frameColor;
			}
			set
			{
				if (!frameColor.Equals(value))
				{
					frameColor = value;	
					flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		private bool ShouldSerializeFrameColor()
		{
			return !frameColor.Equals(SystemColors.Highlight);
		}

		// ************ placement ************

		internal override bool containsPoint(PointF pt)
		{
			ChartObject obj = flowChart.GetObjectAt(pt, true);
			return obj != null && obj.Selected;
		}

		public override bool HitTestHandle(PointF pt, ref int handle)
		{
			if (selectedItems.Count <= 1) return false;

			if (flowChart.ActiveObject != null &&
				flowChart.ActiveObject.HitTestHandle(pt, ref handle))
			{
				handle = 8;
				return true;
			}

			if (style == SelectionStyle.SelectionHandles)
			{
				ChartObject obj = flowChart.GetObjectAt(pt, true);
				if (obj != null)
				{
					handle = 8;
					if (obj.Selected) return true;
				}
			}
			else if (Utilities.pointInRect(pt, rect))
			{
				handle = 8;
				return true;
			}

			return false;
		}

		internal override RectangleF getRepaintRect(bool connected)
		{
			RectangleF result = new RectangleF(0, 0, 0, 0);

			if (!connected)
				result = getBoundingRect();
			else
			{
				RectangleF rc = getBoundingRect();
				InvalidAreaUpdater inv = new InvalidAreaUpdater(rc, false);
				visitHierarchy(inv);

				result = inv.getInvalidRect();
			}

			result.Inflate(flowChart.SelHandleSize, flowChart.SelHandleSize);

			if (selectedBoxes.Count > 0)
			{
				// some boxes might be rotated and their rotation handles
				// be quite outside the selection rectangle
				float infl = 6 * Constants.getMillimeter(flowChart.MeasureUnit);
				result.Inflate(infl, infl);
			}

			return result;
		}


		// ************ mouse pointers ************

		internal override Cursor getCannotDropCursor()
		{
			return flowChart.CurCannotCreate;
		}

		internal override Cursor getCanDropCursor()
		{
			return flowChart.CurPointer;
		}

		public int GetSize()
		{
			return selectedItems.Count;
		}

		internal override bool allowCreate(PointF current)
		{
			// create selection object only if there's more than one selected object
			int objCount = flowChart.getIntrsObjsCnt(rect, includeItemsIfIntersect);
			return (objCount > 0);
		}


		// ************ selected objects ************

		public void Toggle(ChartObject obj)
		{
			if (obj == null) return;

			// remove the object if it's already selected
			if (ObjectInSelection(obj))
			{
				bool activeObjChanged = obj == flowChart.ActiveObject;
				removeObject(obj);
				if (activeObjChanged && selectedItems.Count > 0)
				{
					if (flowChart.ActiveObject != null)
						flowChart.fireDeactivationEvent();
					flowChart.ActiveObject = selectedItems[0];
					flowChart.fireActivationEvent();
				}
				if (activeObjChanged && selectedItems.Count == 0)
				{
					if (flowChart.ActiveObject != null)
						flowChart.fireDeactivationEvent();
					flowChart.ActiveObject = null;
				}

				recalcRect();
			}
			// add to selection
			else if (!obj.notInteractive())
			{
				if (!allowMultiSel) clear();
				addObjToSelection(obj);
				if (flowChart.ActiveObject != null)
					flowChart.fireDeactivationEvent();
				flowChart.ActiveObject = obj;
				flowChart.fireActivationEvent();
			}

			flowChart.fireSelectionChanged();
		}

		internal void addObjToSelection(ChartObject obj)
		{
			if (obj.getSelected()) return;

			selectedItems.Add(obj);
			obj.setSelected(true);

			switch (obj.getType())
			{
				case ItemType.Box:
					selectedBoxes.Add((Box)obj);
					break;
				case ItemType.ControlHost:
					selectedHosts.Add((ControlHost)obj);
					break;
				case ItemType.Table:
					selectedTables.Add((Table)obj);
					break;
				case ItemType.Arrow:
					selectedArrows.Add((Arrow)obj);
					break;
			}

			rect = Utilities.unionNonEmptyRects(rect, obj.getRotatedBounds());
		}

		public void Change(ChartObject obj)
		{
			clear();

			if (obj == null || obj.notInteractive())
			{
				if (flowChart.ActiveObject != null)
					flowChart.fireDeactivationEvent();
				flowChart.ActiveObject = null;
			}
			else
			{
				// change the selection
				addObjToSelection(obj);
				if (flowChart.ActiveObject != null)
					flowChart.fireDeactivationEvent();
				flowChart.ActiveObject = obj;
				flowChart.fireActivationEvent();
			}

			flowChart.fireSelectionChanged();

			recalcRect();
		}

		public void AddObject(ChartObject obj)
		{
			if (obj == null) return;

			addObjToSelection(obj);
			if (flowChart.ActiveObject != null)
				flowChart.fireDeactivationEvent();
			flowChart.ActiveObject = obj;
			flowChart.fireActivationEvent();
			flowChart.invalidate(getRepaintRect(false));

			flowChart.fireSelectionChanged();
		}

		public bool RemoveObject(ChartObject obj)
		{
			RectangleF invArea = getRepaintRect(false);
			if (!removeObject(obj)) return false;
			if (!flowChart.ActiveObject.Selected)
			{
				if (flowChart.ActiveObject != null)
					flowChart.fireDeactivationEvent();
				flowChart.ActiveObject = selectedItems.Count > 0 ? selectedItems[0] : null;
				if (flowChart.ActiveObject != null)
					flowChart.fireActivationEvent();
			}

      if (!flowChart.LayoutSuspended)
        flowChart.invalidate(invArea);

			flowChart.fireSelectionChanged();

			return true;
		}

		internal bool removeObject(ChartObject obj)
		{
			bool objExists = ObjectInSelection(obj);
			if (objExists)
			{
				selectedItems.Remove(obj);
				switch (obj.getType())
				{
					case ItemType.Box:
						selectedBoxes.Remove((Box)obj);
						break;
					case ItemType.ControlHost:
						selectedHosts.Remove((ControlHost)obj);
						break;
					case ItemType.Table:
						selectedTables.Remove((Table)obj);
						break;
					case ItemType.Arrow:
						selectedArrows.Remove((Arrow)obj);
						break;
				}
				obj.setSelected(false);

				recalcRect();
			}

			return objExists;
		}

		public bool ObjectInSelection(ChartObject obj)
		{
			foreach (ChartObject selObj in selectedItems)
				if (selObj == obj) return true;

			return false;
		}

		private void clear()
		{
			foreach (ChartObject obj in selectedItems)
				obj.setSelected(false);

			selectedItems.Clear();

			selectedBoxes.Clear();
			selectedHosts.Clear();
			selectedTables.Clear();
			selectedArrows.Clear();
		}

		public void Clear()
		{
			int oldSelCount = selectedItems.Count;

			flowChart.invalidate(getRepaintRect(false));
			clear();
			recalcRect();
			flowChart.ActiveObject = null;

			if (oldSelCount > 0)
				flowChart.fireSelectionChanged();
		}

		internal override void visitHierarchy(CollectionVisitor visitor)
		{
			foreach (ChartObject obj in selectedItems)
			{
				if (!parentSelected(obj, true))
					obj.visitHierarchy(visitor);
			}
		}

		private void updateNSARect(ChartObject obj)
		{
			RectangleF rcObj = obj.getRepaintRect(true);

			if (obj.getType() == ItemType.Arrow)
			{
				Arrow arrow = (Arrow)obj;
				arrow.updatePosFromOrgAndDest(false);
			}
		}

		private bool parentSelected(ChartObject obj, bool checkCycles)
		{
			// for arrows check if the objects they are linked to are selected
			if (obj.getType() == ItemType.Arrow)
			{
				Arrow arrow = (Arrow)obj;
				if (arrow.getOrgnLink().getNode().Selected) return true;
				if (arrow.getDestLink().getNode().Selected) return true;
			}

			// doesn't have parent at all
			if (!obj.isAttached()) return false;

			// has a parent, but it is not selected
			if (!obj.getContainingGroup().MainObject.Selected)
				return false;

			// is the group master in a cycle
			if (!checkCycles)
			{
				if (obj.getGroupMaster() == obj) return false;
			}
			else
			{
				if (obj.getGroupMasterCC() == obj) return false;
			}

			// true in all other cases
			return true;
		}

		/// <summary>
		/// checks whether an item shouldn't be moved by the Selection object itself, either because
		/// its group master moves it, or if it's an arrow whose origin or dest. nodes aren't modified;
		/// this method is intended to be called only from updateModify
		/// </summary>
		private bool ignoreItem(ChartObject item, InteractionState ist)
		{
			if (item is Node && item.MasterGroup != null)
			{
				// ignore the item if its group master is already moving
				if (item.MasterGroup.MainObject.getModifying() &&
					!ist.cycleRoots.Contains(item))
					return true;
			}

			// ignore arrows whose source and destination aren't modified
			Arrow arrow = item as Arrow;
			if (arrow != null)
			{
				if (!arrow.Origin.getModifying() && !arrow.Destination.getModifying())
					return true;
			}

			return parentSelected(item, false);
		}

		private bool HV_ModifyTranslate(ChartObject obj)
		{
			obj.modifyTranslate(modifyDX, modifyDY, false);
			return true;
		}

		private SelectionStyle style;

		[Browsable(false)]
		public ChartObjectCollection Objects
		{
			get { return selectedItems; }
		}

		/// <summary>
		/// Collection of the selected arrows
		/// </summary>
		[Browsable(false)]
		public ArrowCollection Arrows
		{
			get
			{
				return selectedArrows;
			}
		}

		/// <summary>
		/// Collection of the selected boxes
		/// </summary>
		[Browsable(false)]
		public BoxCollection Boxes
		{
			get
			{
				return selectedBoxes;
			}
		}

		[Browsable(false)]
		public ControlHostCollection ControlHosts
		{
			get
			{
				return selectedHosts;
			}
		}

		/// <summary>
		/// Collection of the selected tables
		/// </summary>
		[Browsable(false)]
		public TableCollection Tables
		{
			get
			{
				return selectedTables;
			}
		}

		private void recalcRect()
		{
			rect = new RectangleF(0, 0, 0, 0);
			foreach (ChartObject obj in selectedItems)
				rect = Utilities.unionNonEmptyRects(rect, obj.getRotatedBounds());
		}

		/// <summary>
		/// Visual style of multiple selection
		/// </summary>
		[Browsable(true)]
		[DefaultValue(typeof(SelectionStyle), "SelectionHandles")]
		[Description("Specifies how multiple selection should look.")]
		public SelectionStyle Style
		{
			get
			{
				return style;
			}
			set
			{
				if (style != value)
				{
					style = value;
					flowChart.setDirty();
					if (!flowChart.LayoutSuspended)
            flowChart.invalidate(getRepaintRect(false));
				}
			}
		}

		public override int getClassId()
		{
			return 8;
		}

		public override void saveTo(BinaryWriter writer, PersistContext ctx)
		{
			base.saveTo(writer, ctx);
			ctx.saveReference(this, selectedItems, 1);
			ctx.saveReference(this, selectedBoxes, 2);
			ctx.saveReference(this, selectedTables, 3);
			ctx.saveReference(this, selectedArrows, 4);

			writer.Write((int)style);

			// new in file format 12
			ctx.saveReference(this, selectedHosts, 5);
		}

		public override void loadFrom(BinaryReader reader, PersistContext ctx)
		{
			base.loadFrom(reader, ctx);
			ctx.loadReference(this);
			ctx.loadReference(this);
			ctx.loadReference(this);
			ctx.loadReference(this);
			style = (SelectionStyle)reader.ReadInt32();

			if (ctx.FileVersion >= 12)
			{
				// new in file format 12
				ctx.loadReference(this);	// selectedHosts
			}
		}

		public override void setReference(int refId, IPersists obj)
		{
			switch (refId)
			{
			case 1:
				selectedItems = (ChartObjectCollection)obj;
				break;
			case 2:
				selectedBoxes = (BoxCollection)obj;
				break;
			case 3:
				selectedTables = (TableCollection)obj;
				break;
			case 4:
				selectedArrows = (ArrowCollection)obj;
				break;
			case 5:
				selectedHosts = (ControlHostCollection)obj;
				break;
			}
		}

		// enables or disables multiple-selection
		[Browsable(true)]
		[DefaultValue(true)]
		[Description("Specifies whether users are allowed to select multiple items.")]
		public bool AllowMultiSel
		{
			get { return allowMultiSel; }
			set { allowMultiSel = value; }
		}

		private bool allowMultiSel;

		void HV_TranslateArrow(ChartObject obj)
		{
			if (obj.getType() == ItemType.Arrow)
			{
				Arrow arrow = (Arrow)obj;

				if (arrow.bothEndsSelected() && !arrowsToMove.Contains(arrow) &&
					arrow.Origin.canModify(8) && arrow.Destination.canModify(8))
				{
					arrowsToMove.Add(arrow);
					return;
				}

				if (arrow.Origin.getModifying() && arrow.Destination.getModifying() &&
					!arrowsToMove.Contains(arrow) && relatedInteraction != null &&
					!relatedInteraction.affectedArrows.Contains(arrow))
				{
					arrowsToMove.Add(arrow);
					return;
				}
			}
		}

		internal SelectionState saveState()
		{
			SelectionState state = new SelectionState();

			state.SelectedItems = Objects.Clone();
			state.SelectedBoxes = Boxes.Clone();
			state.SelectedTables = Tables.Clone();
			state.SelectedArrows = Arrows.Clone();
			state.SelectedHosts = ControlHosts.Clone();
			state.ActiveItem = flowChart.ActiveObject;

			return state;
		}

		internal void restoreState(SelectionState state)
		{
			foreach (ChartObject obj in selectedItems)
				if (!state.SelectedItems.Contains(obj))
					obj.setSelected(false);

			selectedItems = state.SelectedItems.Clone();
			selectedBoxes = state.SelectedBoxes.Clone();
			selectedTables = state.SelectedTables.Clone();
			selectedArrows = state.SelectedArrows.Clone();
			selectedHosts = state.SelectedHosts.Clone();
			flowChart.ActiveObject = state.ActiveItem;

			foreach (ChartObject obj in selectedItems)
				obj.setSelected(true);
		}

		private ChartObjectCollection selectedItems;
		private BoxCollection selectedBoxes;
		private ControlHostCollection selectedHosts;
		private TableCollection selectedTables;
		private ArrowCollection selectedArrows;

		private ChartObjectCollection arrowsToMove;

		private PointF ptLastTopLeft;

		/// <summary>
		/// Returns the arrow bounding rectangle
		/// </summary>
		[Browsable(false)]
		public RectangleF BoundingRect
		{
			get { return getBoundingRect(); }
		}

		public bool IncludeItemsIfIntersect
		{
			get { return includeItemsIfIntersect; }
			set { includeItemsIfIntersect = value; }
		}

		private bool includeItemsIfIntersect;

		private InteractionState relatedInteraction;
	}
}