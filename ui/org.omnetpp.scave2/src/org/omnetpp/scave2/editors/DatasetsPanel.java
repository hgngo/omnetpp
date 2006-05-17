package org.omnetpp.scave2.editors;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.eclipse.ui.forms.widgets.FormToolkit;

public class DatasetsPanel extends TreeWithButtonsPanel {
	
	private Button openDatasetButton;
	private Button newDatasetButton;
	private Button removeDatasetButton;
	
	public DatasetsPanel(Composite parent, int style) {
		super(parent, style);
		initialize();
	}
	
	private void initialize() {
		Label label = getLabel();
		Composite buttonPanel = getButtonPanel();
		FormToolkit toolkit = getFormToolkit();
		label.setText("Here you can browse the datasets you have created from the input.");
		openDatasetButton = toolkit.createButton(buttonPanel, "Open...", SWT.NONE);
		newDatasetButton = toolkit.createButton(buttonPanel, "New...", SWT.NONE);
		removeDatasetButton = toolkit.createButton(buttonPanel, "Remove", SWT.NONE);
	}
}
