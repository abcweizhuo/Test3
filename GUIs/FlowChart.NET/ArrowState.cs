// Copyright (c) 2003-2006, MindFusion Limited - Gibraltar.
// This source code is provided to you as part of the FlowChart.NET control software
// package you have purchased. Its purpose is to help you trace and/or fix
// problems or customize the Control as needed for your application. To get permission
// to use the sources in any other way, please contact us at info@mindfusion.org
// Redistribution or any usage of the sources in a way not mentioned above is
// illegal and shall be pursued and punished with all means provided by Copyright laws.

using System;
using System.Drawing;


namespace MindFusion.FlowChartX
{
	/// <summary>
	/// 
	/// </summary>
	internal class ArrowState : ItemState
	{
		internal ArrowState(Arrow arrow) :base(arrow)
		{
			points = null;
		}

		internal bool cascadeStartHorizontal;
		internal bool reflexive;
		internal PointCollection points;
		internal short segmentCount;
		internal ArrowStyle style;

		internal Link orgnLink;
		internal int orgnAnchor;
		internal Link destLink;
		internal int destAnchor;

		internal PointF orgnPoint;
		internal PointF destPoint;
	}
}