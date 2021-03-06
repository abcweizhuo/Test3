
using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace SysCAD.Editor
{
  static class Program
  {
    /// <summary>
    /// The main entry point for the application.
    /// </summary>
    [STAThread]
    static void Main()
    {
      Application.EnableVisualStyles();
      Application.SetCompatibleTextRenderingDefault(false);

      EditorForm editorForm = new EditorForm();

      while (editorForm != null)
      {
#if (DEBUG)
        try
        {
          Application.Run(editorForm);
          editorForm = null;
        }
        catch (Exception e)
        {
          DialogResult dialogResult;
          if (e is SysCAD.Protocol.ConnectionLostException)
            dialogResult = MessageBox.Show("The connection with the server was lost.", "Connection with server lost.", MessageBoxButtons.RetryCancel);
          else
            dialogResult = ShowStackTraceBox(e, editorForm);
          
          editorForm.Dispose();

          if (dialogResult == DialogResult.Retry)
            editorForm = new EditorForm();
          else
            editorForm = null;
        }
#else
        Application.Run(editorForm);
        editorForm = null;
#endif
      }
    }

    private static DialogResult ShowStackTraceBox(Exception e, EditorForm editorForm)
    {
      string messagePre = string.Empty;
      string messageBody = string.Empty;

      messagePre += "An error has occurred.  Detailed debug information has been copied into the clipboard.\n";
      messagePre += "Please add to bugzilla or email paul.hannah@syscad.net a copy of this debug information\n";
      messagePre += "along with what you were doing.\n\n";

      messageBody += "Exception:\n" + e.ToString() + "\n\n";
      if ((e.Data != null) && (e.Data.Count > 0))
      {
        messageBody += "Extra details:\n";
        foreach (System.Collections.DictionaryEntry de in e.Data)
          messageBody += "The key is '" + de.Key + "' and the value is: " + de.Value + "\n";
        messageBody += "\n";
      }
      if (e.InnerException != null)
      {
        messageBody += "Inner exception message:\n" + e.InnerException.Message + "\n\n";
        if (e.InnerException.Data != null)
        {
          messageBody += "Extra details:";
          foreach (System.Collections.DictionaryEntry de in e.InnerException.Data)
            messageBody += "The key is '" + de.Key + "' and the value is: " + de.Value + "\n";
          messageBody += "\n";
        }
      }

      if (editorForm != null)
      {
        if (editorForm.FrmFlowChart != null)
        {
          if (editorForm.FrmFlowChart.State != null)
          {
            if (editorForm.FrmFlowChart.State.Config != null)
            {
              if (editorForm.FrmFlowChart.State.Config.GraphicStencils != null)
              {
                messageBody += "GraphicStencils:\n";
                foreach (SysCAD.Protocol.GraphicStencil graphicStencil in editorForm.FrmFlowChart.State.Config.GraphicStencils.Values)
                  messageBody += "Tag: " + graphicStencil.Tags[0] + "\n";
                messageBody += "\n";
              }
              if (editorForm.FrmFlowChart.State.Config.ModelStencils != null)
              {
                messageBody += "ModelStencils:\n";
                foreach (SysCAD.Protocol.ModelStencil modelStencil in editorForm.FrmFlowChart.State.Config.ModelStencils.Values)
                  messageBody += "Tag: " + modelStencil.Tag + "\n";
                messageBody += "\n";
              }
              if (editorForm.FrmFlowChart.State.Config.ProjectList != null)
              {
                messageBody += "ProjectList:\n";
                foreach (string project in editorForm.FrmFlowChart.State.Config.ProjectList)
                  messageBody += "Project: " + project + "\n";
                messageBody += "\n";
              }
            }
            if (editorForm.FrmFlowChart.State.ClientProtocol != null)
            {
              if (editorForm.FrmFlowChart.State.ClientProtocol.graphic != null)
              {
                if (editorForm.FrmFlowChart.State.ClientProtocol.graphic.Areas != null)
                {
                  messageBody += "Areas:\n";
                  foreach (SysCAD.Protocol.GraphicArea area in editorForm.FrmFlowChart.State.ClientProtocol.graphic.Areas.Values)
                  {
                    messageBody += "Tag: " + area.Tag + "\n";
                    messageBody += "Path: " + area.Path + "\n";
                    messageBody += "Guid: " + area.Guid.ToString() + "\n";
                    messageBody += "Rect: " + area.BoundingRect.ToString() + "\n";
                    messageBody += "\n";
                  }
                  messageBody += "\n";
                }
                if (editorForm.FrmFlowChart.State.ClientProtocol.graphic.Nodes != null)
                {
                  messageBody += "Nodes:\n";
                  foreach (SysCAD.Protocol.GraphicNode node in editorForm.FrmFlowChart.State.ClientProtocol.graphic.Nodes.Values)
                  {
                    messageBody += "Tag: " + node.Tag + "\n";
                    messageBody += "Path: " + node.Path + "\n";
                    messageBody += "Guid: " + node.Guid.ToString() + "\n";
                    messageBody += "Rect: " + node.BoundingRect.ToString() + "\n";
                    messageBody += "\n";
                  }
                  messageBody += "\n";
                }
                if (editorForm.FrmFlowChart.State.ClientProtocol.graphic.Links != null)
                {
                  messageBody += "Links:\n";
                  foreach (SysCAD.Protocol.GraphicLink link in editorForm.FrmFlowChart.State.ClientProtocol.graphic.Links.Values)
                  {
                    messageBody += "Tag: " + link.Tag + "\n";
                    messageBody += "Origin: " + link.Origin + "\n";
                    messageBody += "OriginPortID: " + link.OriginPortID + "\n";
                    messageBody += "Destination: " + link.Destination + "\n";
                    messageBody += "DestinationPortID: " + link.DestinationPortID + "\n";
                    messageBody += "Guid: " + link.Guid.ToString() + "\n";
                    messageBody += "ControlPoints: " + link.ControlPoints.ToString() + "\n";
                    messageBody += "\n";
                  }
                  messageBody += "\n";
                }
              }
            }
          }
        }
      }

      Clipboard.SetText(messageBody);

      return MessageBox.Show(messagePre, "An error has occurred.", MessageBoxButtons.RetryCancel);
    }
  }
}
