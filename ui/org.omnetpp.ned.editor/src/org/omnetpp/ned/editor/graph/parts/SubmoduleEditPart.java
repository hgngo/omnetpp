/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.parts;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.ConnectionAnchor;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.gef.commands.Command;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.layout.SubmoduleConstraint;
import org.omnetpp.ned.editor.graph.commands.ReconnectCommand;
import org.omnetpp.ned.editor.graph.figures.SubmoduleFigureEx;
import org.omnetpp.ned.editor.graph.properties.util.SubmoduleNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;


/**
 * Controls the submodule figure according to the model changes.
 *
 * @author rhornig
 */
public class SubmoduleEditPart extends ModuleEditPart {
    protected GateAnchor gateAnchor;

    @Override
    public void activate() {
        super.activate();
        renameValidator = new SubmoduleNameValidator(getSubmoduleModel());
    }

    @Override
    protected void createEditPolicies() {
        super.createEditPolicies();
    }
    /**
     * Returns a newly created Figure of this.
     */
    @Override
    protected IFigure createFigure() {
        SubmoduleFigure fig = new SubmoduleFigureEx();
        // set the pin decoration image for the image (The compound module requests an auto-layout
        // if we add an figure without pin. ie. submodule created in the text editor without 
        // a display string
        fig.setPinDecoration(getSubmoduleModel().getDisplayString().getLocation(1.0f) != null);

        gateAnchor = new GateAnchor(fig);
        return fig;
    }

    /**
     * Returns the Figure for this as an SubmoduleFigure.
     */
    public SubmoduleFigure getSubmoduleFigure() {
        return (SubmoduleFigure)getFigure();
    }

    /**
     * Helper function to return the model object with correct type
     */
    public SubmoduleElementEx getSubmoduleModel() {
        return (SubmoduleElementEx)getModel();
    }

	/**
	 * Compute the source connection anchor to be assigned based on the current mouse
	 * location and available gates.
	 * @param p current mouse coordinates
	 * @return The selected connection anchor
	 */
	@Override
    public ConnectionAnchor getConnectionAnchorAt(Point p) {
        return gateAnchor;
	}

	/**
	 * Returns a connection anchor registered for the given gate
	 */
	@Override
    public GateAnchor getConnectionAnchor(String gate) {
        return gateAnchor;
	}

    /**
     * Returns a list of connections for which this is the srcModule.
     */
    @SuppressWarnings("unchecked")
    @Override
    protected List getModelSourceConnections() {
        // get the connections from out controller parent's model
        return getCompoundModulePart().getCompoundModuleModel().getSrcConnectionsFor(getSubmoduleModel().getName());
    }

    /**
     * Returns a list of connections for which this is the destModule.
     */
    @SuppressWarnings("unchecked")
    @Override
    protected List getModelTargetConnections() {
        // get the connections from out controller parent's model
        return getCompoundModulePart().getCompoundModuleModel().getDestConnectionsFor(getSubmoduleModel().getName());
    }

    /**
     * Updates the visual aspect of this.
     */
    @Override
    protected void refreshVisuals() {
        super.refreshVisuals();
        // define the properties that determine the visual appearance
    	SubmoduleElementEx submNode = getSubmoduleModel();

    	// set module name and vector size
    	String nameToDisplay = submNode.getName();
    	// add [size] if it's a module vector
    	if (StringUtils.isNotEmpty(submNode.getVectorSize()))
    		nameToDisplay += "["+submNode.getVectorSize()+"]";
    	getSubmoduleFigure().setName(nameToDisplay);

    	// parse a display string, so it's easier to get values from it.
    	// for other visual properties
        DisplayString dps = submNode.getDisplayString();

        // get the scale factor for this submodule (coming from the containing compound module's display string)
        float scale = getScale();
        // set it in the figure, so size and range indicator can use it
        getSubmoduleFigure().setScale(scale);

        // set the rest of the display properties
        getSubmoduleFigure().setDisplayString(dps);
        
        getSubmoduleFigure().setQueueText(StringUtils.isNotBlank(dps.getAsString(IDisplayString.Prop.QUEUE)) ? "#" : "");

        // set layout constraints
        // use the existing constraint
        SubmoduleConstraint constraint = getSubmoduleFigure().getSubmoduleConstraint();
        if (constraint == null)                      // or create a new one if no constraint was created until now
        	constraint = new SubmoduleConstraint();  // the location is unspecified in this constraint by default
        
        Point dpsLoc = dps.getLocation(scale);
        // check if the figure has a display string that specified a location (figure is fixed)
        constraint.setPinned(dpsLoc != null);
        // use the location specified in the display string (if any) for pinned nodes,
        // otherwise just use the current position stored in the constraint (which can be either unspecified or
        // can store the result from the previous layout run)
        if (constraint.isPinned())
        	constraint.setLocation(dpsLoc);

        // use the preferred size of the figure for constraint size
        constraint.setSize(getSubmoduleFigure().getPreferredSize());
        Assert.isTrue(constraint.height != -1 && constraint.width != -1);        
        // Debug.println("constraint for " + nameToDisplay + ": " + constraint);
        getSubmoduleFigure().setSubmoduleConstraint(constraint);  // store the constraint (needed if a new constraint was created)
        
        // show/hide the pin marker
        getSubmoduleFigure().setPinDecoration(dpsLoc != null);
    }

    @Override
    public float getScale() {
        // get the container compound module's scaling factor
        return getCompoundModulePart().getScale();
    }

    @Override
    public boolean isEditable() {
        // editable only if the parent controllers model is the same as the model's parent
        // i.e. the submodule is defined in this compound module (not inherited)
        return super.isEditable() && getParent().getModel() == ((SubmoduleElementEx)getModel()).getCompoundModule();
    }

    @Override
    protected Command validateCommand(Command command) {
        // connection creation is allowed even if the submodule is non editable (but the containing
        // compound module is editable)
        if (command instanceof ReconnectCommand && getCompoundModulePart().isEditable())
            return command;

        return super.validateCommand(command);
    }

    @Override
    public CompoundModuleEditPart getCompoundModulePart() {
        return (CompoundModuleEditPart)getParent();
    }

    @Override
    protected INEDElement getNEDElementToOpen() {
        return getSubmoduleModel().getEffectiveTypeRef();
    }
}
