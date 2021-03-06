// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Resources;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// Implements drawing and hit testing for the the expand (+) and 
	/// collapse (-) icons displayed beside expandable boxes.
	/// </summary>
	internal sealed class Expander : MindFusion.FlowChartX.Manipulator
	{
		internal Expander(ChartObject item) : base(item)
		{
			if (imgExpanded == null || imgCollapsed == null)
			{
				ResourceManager rm = new ResourceManager(
					"MindFusion.FlowChartX.Icons", this.GetType().Assembly);
				imgExpanded = (Icon)rm.GetObject("mnp_collapse");
				imgCollapsed = (Icon)rm.GetObject("mnp_expand");
			}
		}

		internal override void onDelete()
		{
			base.onDelete();
		}

		internal override void draw(Graphics g)
		{
			RectangleF rcNode = item.getRotatedBounds();
			Rectangle rcDev = Utilities.docToDevice(g, getIconRect(rcNode));

			if (rcDev.Width < 6 || rcDev.Height < 6) return;
GraphicsState state = g.Save();
item.flowChart.unsetTransforms(g);
			if ((item as Node).Expanded)
				g.DrawIcon(imgExpanded, rcDev.X, rcDev.Y);
			else
				g.DrawIcon(imgCollapsed, rcDev.X, rcDev.Y);
g.Restore(state);
		}

		internal override void addToRepaintRect(ref RectangleF rect)
		{
			float exp = Constants.getExpandIconSize(item.flowChart.MeasureUnit) * 4 / 3;

			switch (item.flowChart.ExpandButtonPosition)
			{
				case ExpandButtonPosition.OuterUpperRight:
				case ExpandButtonPosition.OuterRight:
				case ExpandButtonPosition.OuterLowerRight:
					rect.Width += exp;
					break;
				case ExpandButtonPosition.OuterBottom:
					rect.Height += exp;
					break;
				case ExpandButtonPosition.OuterLeft:
				case ExpandButtonPosition.OuterLowerLeft:
				case ExpandButtonPosition.OuterUpperLeft:
					rect.X -= exp;
					rect.Width += exp;
					break;
				case ExpandButtonPosition.OuterTop:
					rect.Y -= exp;
					rect.Height += exp;
					break;
			}
		}

		private PointF iconPos(RectangleF nodeRect)
		{
			float exp = Constants.getExpandIconSize(item.flowChart.MeasureUnit);
			PointF ipos = PointF.Empty;

			switch (item.flowChart.ExpandButtonPosition)
			{
				case ExpandButtonPosition.OuterRight:
					ipos.X = nodeRect.Right + exp / 3;
					ipos.Y = nodeRect.Top + nodeRect.Height / 2 - exp / 2;
					break;
				case ExpandButtonPosition.OuterLowerRight:
					ipos.X = nodeRect.Right + exp / 3;
					ipos.Y = nodeRect.Bottom - exp;
					break;
				case ExpandButtonPosition.OuterUpperRight:
					ipos.X = nodeRect.Right + exp / 3;
					ipos.Y = nodeRect.Top;
					break;
				case ExpandButtonPosition.OuterBottom:
					ipos.X = nodeRect.Left + nodeRect.Width / 2 - exp / 2;
					ipos.Y = nodeRect.Bottom + exp / 3;
					break;
				case ExpandButtonPosition.OuterLeft:
					ipos.X = nodeRect.Left - exp / 3 - exp;
					ipos.Y = nodeRect.Top + nodeRect.Height / 2 - exp / 2;
					break;
				case ExpandButtonPosition.OuterLowerLeft:
					ipos.X = nodeRect.Left - exp / 3 - exp;
					ipos.Y = nodeRect.Bottom - exp;
					break;
				case ExpandButtonPosition.OuterUpperLeft:
					ipos.X = nodeRect.Left - exp / 3 - exp;
					ipos.Y = nodeRect.Top;
					break;
				case ExpandButtonPosition.OuterTop:
					ipos.X = nodeRect.Left + nodeRect.Width / 2 - exp / 2;
					ipos.Y = nodeRect.Top - exp / 3 - exp;
					break;
			}

			return ipos;
		}

		private RectangleF getIconRect(RectangleF nodeRect)
		{
			return new RectangleF(iconPos(nodeRect),
				new SizeF(Constants.getExpandIconSize(item.flowChart.MeasureUnit), Constants.getExpandIconSize(item.flowChart.MeasureUnit)));
		}

		/// <summary>
		/// Determines whether the +/- button is clicked and expands or
		/// collapses the tree branch that starts from the node if so ...
		/// </summary>
		/// <param name="point">The document coordinates of the mouse pointer.</param>
		/// <returns>true if the expand/collapse button has been clicked.</returns>
		internal override bool hitTest(PointF point)
		{
			RectangleF bounds = item.getRotatedBounds();
			RectangleF iconRect = getIconRect(bounds);

			if (Utilities.pointInRect(point, iconRect))
			{
				FlowChart fc = item.flowChart;
				Node node = item as Node;

				if (fc.ExpandButtonAction == ExpandButtonAction.ExpandTreeBranch)
				{
					fc.UndoManager.onStartExpand();

					if (node.Expanded)
					{
						// collapse the tree branch groiwng from this node
						node.collapse();
						fc.fireTreeCollapsed(node);
					}
					else
					{
						// expand the tree branch groiwng from this node
						node.expand();
						fc.fireTreeExpanded(node);
					}

					fc.UndoManager.onEndExpand();
				}

				if (fc.ExpandButtonAction == ExpandButtonAction.RaiseEvents)
				{
					node.SetExpandedFlag(!node.isExpanded());
					fc.raiseExpandBtnClicked(node);
				}

				return true;
			}

			return false;
		}

		internal override bool ptInManipulator(PointF pt)
		{
			if (Utilities.pointInRect(pt, getBounds()))
				return true;

			return false;
		}

		internal RectangleF getBounds()
		{
			RectangleF rcNode = item.getRotatedBounds();
			return getIconRect(rcNode);
		}

		internal override bool supportClipping()
		{
			return false;
		}

		private static Icon imgExpanded = null;
		private static Icon imgCollapsed = null;
	}
}