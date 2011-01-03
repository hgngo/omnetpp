/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.figures;

import org.eclipse.draw2d.ConnectionLocator;
import org.eclipse.draw2d.Graphics;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.Label;
import org.eclipse.draw2d.MidpointLocator;
import org.eclipse.draw2d.PolygonDecoration;
import org.eclipse.draw2d.PolylineConnection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.figures.misc.ConnectionLabelLocator;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
//TODO only parse display string if it's changed; ditto for all public setters
public class ConnectionFigure extends PolylineConnection {
	protected int localLineStyle = Graphics.LINE_SOLID;
	protected int localLineWidth = 1;
	protected Color localLineColor = null;
    protected Label textFigure = new Label() {
        // KLUDGE: we don't want to allow selecting a connection by clicking on its visible label
        // NOTE: overriding findFigureAt does not work
        @Override
        public boolean containsPoint(int x, int y) {
            return false;
        };
    };
    protected ConnectionLabelLocator labelLocator = new ConnectionLabelLocator(this);
    protected TooltipFigure tooltipFigure;
	protected boolean isArrowHeadEnabled;
    protected IFigure centerDecoration;

	@Override
    public void addNotify() {
        super.addNotify();
        add(textFigure, labelLocator);
    }
    public Color getLocalLineColor() {
		return localLineColor;
	}

	public int getLocalLineStyle() {
		return localLineStyle;
	}

	public int getLocalLineWidth() {
		return localLineWidth;
	}

	public void setArrowHeadEnabled(boolean arrowHeadEnabled) {
		if (this.isArrowHeadEnabled != arrowHeadEnabled) {
			this.isArrowHeadEnabled = arrowHeadEnabled;
			if (arrowHeadEnabled) {
				if (getTargetDecoration() == null) {
					PolygonDecoration arrow = new PolygonDecoration();
					arrow.setTemplate(PolygonDecoration.TRIANGLE_TIP);
					setTargetDecoration(arrow);
				}
			}
			if (getTargetDecoration() != null)
				getTargetDecoration().setVisible(arrowHeadEnabled);
	    }
    }

	public void setMidpointDecoration(IFigure decoration) {
	    if (centerDecoration == decoration)
	        return;
	    if (centerDecoration != null)
	        remove(centerDecoration);
	    centerDecoration = decoration;
	    if (centerDecoration != null)
	        add(centerDecoration, new MidpointLocator(this, 0));
	}

    protected void setStyle(Color color, int width, String style) {
    	style = style == null ? "" : style;
    	if (style.toLowerCase().startsWith("da"))
    		setLineStyle(localLineStyle = Graphics.LINE_DASH);
    	else if (style.toLowerCase().startsWith("d"))
    		setLineStyle(localLineStyle = Graphics.LINE_DOT);
    	else
    		setLineStyle(localLineStyle = Graphics.LINE_SOLID);

    	setLineWidth(localLineWidth = width);
    	setForegroundColor(localLineColor = color);
    	// arrow scaling proportional with the line width
		if (getTargetDecoration() != null) {
            ((PolygonDecoration)getTargetDecoration()).setScale(5+lineWidth, 2+lineWidth);
		    ((PolygonDecoration)getTargetDecoration()).setLineWidth(lineWidth);
		}
    }

    public void setTooltipText(String tttext) {
        if (tttext == null || "".equals(tttext)) {
            setToolTip(null);
            tooltipFigure = null;
        }
        else {
            tooltipFigure = new TooltipFigure();
            setToolTip(tooltipFigure);
            tooltipFigure.setText(tttext);
            invalidate();
        }
    }

    protected void setInfoText(String text, String alignment, Color color) {

        if (textFigure == null)
            return;
        boolean isEmpty = StringUtils.isEmpty(text);
        textFigure.setVisible(!isEmpty);
        // we add an extra space, because label ends might be clipped because of antialiasing
        textFigure.setText(isEmpty ? "" : text + " ");
        textFigure.setForegroundColor(color);
        // set position
        if (alignment != null) {
            if (alignment.startsWith("l")) {
                labelLocator.setAlignment(ConnectionLocator.SOURCE);
            }
            else if (alignment.startsWith("r")) {
                labelLocator.setAlignment(ConnectionLocator.TARGET);
            } else
                labelLocator.setAlignment(ConnectionLocator.MIDDLE);
        }
        revalidate();
    }

    /**
	 * Adjusts the figure properties using a displayString object
	 * @param dps The display string object containing the properties
	 */
	public void setDisplayString(IDisplayString dps) {
        setStyle(ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.CONNECTION_COLOR)),
				dps.getAsInt(IDisplayString.Prop.CONNECTION_WIDTH, 1),
				dps.getAsString(IDisplayString.Prop.CONNECTION_STYLE));
        // tooltip support
        setTooltipText(dps.getAsString(IDisplayString.Prop.TOOLTIP));
        // additional text support
        setInfoText(dps.getAsString(IDisplayString.Prop.TEXT),
                dps.getAsString(IDisplayString.Prop.TEXT_POS),
                ColorFactory.asColor(dps.getAsString(IDisplayString.Prop.TEXT_COLOR)));

        invalidate();
	}

	@Override
	public void paint(Graphics graphics) {
	    graphics.pushState();
	    // zero width connections do not appear (but we display them almost invisible for convenience
        if (getLineWidth() < 1)
            graphics.setAlpha(16);

        graphics.setLineCap(SWT.CAP_FLAT);
	    super.paint(graphics);
	    graphics.popState();
	}
}
