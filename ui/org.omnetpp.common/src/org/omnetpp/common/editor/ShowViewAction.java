package org.omnetpp.common.editor;

import org.eclipse.jface.action.Action;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.views.IViewDescriptor;
import org.omnetpp.common.CommonPlugin;


/**
 * Action to show a given view.
 * @author Andras
 */
public class ShowViewAction extends Action {
	private String viewID;

	public ShowViewAction(String viewID) {
		IViewDescriptor viewDesc = PlatformUI.getWorkbench().getViewRegistry().find(viewID);
		if (viewDesc == null) 
			throw new IllegalArgumentException("No such view registered: "+viewID);
		this.viewID = viewID;
		setText("Show " + viewDesc.getLabel());
		setToolTipText("Open the "+viewDesc.getLabel()+" view");
		setImageDescriptor(viewDesc.getImageDescriptor());
	}

	@Override
	public void run() {
		try {
			IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
			workbenchPage.showView(viewID);
		} 
		catch (PartInitException e) {
			CommonPlugin.logError(e);
		}
	}
}
