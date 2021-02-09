% File: DetectOverUnderFlow.m
%
% Description: plot a dot on the screen for each data point that goes above
% or below the currently chosen scaling
% 
% Parameter(s): 
%     X - X for current plot
%     Y - Y to detect over or under flow for
%     InputStruct - optional struct to contain options and flags
% 
% Return value: void
% 
% Usage: 
% X = 1:10;
% Y = 11:20;
% DetectOverUnderFlow(X, Y)

function [] = DetectOverUnderFlow(X,Y,InputStruct)

%     If no InputStruct parameter, create a blank one    
    if (nargin <3)
        InputStruct = [];
    end
    
%     Note: there is currently no matching "hold off", which doesn't appear
%     to be a problem, but is living slightly dangerously
    hold on
    
%         if InputStruct has a field 'SpecifiedAxes', then call the axis command with those axes 
%               to get the current axis values
    if (isfield(InputStruct,'SpecifiedAxes'))
        AxisValues = axis(InputStruct.SpecifiedAxes);
    else
        AxisValues = axis;  
    end     
    
    xmin = AxisValues(1);
    xmax = AxisValues(2);
    xrange = xmax-xmin;
    ymin = AxisValues(3);
    ymax = AxisValues(4);
    yrange = ymax-ymin;
    
%     If the axes to plot dots on is NOT the axis to detect over under flow for
    if (isfield(InputStruct,'PlottingAxes'))
        OtherAxisValues = axis(InputStruct.PlottingAxes);

        Otherxmin = OtherAxisValues(1);
        Otherxmax = OtherAxisValues(2);
        Otherxrange = Otherxmax-Otherxmin;
        Otherymin = OtherAxisValues(3);
        Otherymax = OtherAxisValues(4);
        Otheryrange = Otherymax-Otherymin;
    end

    TextYMinOffset = 0.015;
    if (isfield(InputStruct,'TextYMinOffset'))
        TextYMinOffset = InputStruct.TextYMinOffset;
    end    
    
    TextYMaxOffset = 0.01;
    if (isfield(InputStruct,'TextYMaxOffset'))
        TextYMaxOffset = InputStruct.TextYMaxOffset;
    end    
    
    sz = size(Y);
    ymaxoffset = 0.*X;
    yminoffset = 0.*X;
    for i=1:sz(2)
        
%           Use color matching the data point that is off screen        
        Order = get(gcf,'DefaultAxesColorOrder'); %GetDefaultAxesColorOrder(gcf);
%         Color = [1 1 0];
        sz = size(Order);
        if (sz(1) == 1)
            Color = Order;
        else
            Color = Order(1+mod(i-1,length(Order)),:);
        end
        
%         Overflow
        f = find(Y(:,i)>ymax);
        for j = 1:length(f)
            ind = f(j);
            if (ymaxoffset(ind) == 2) && (i == 3)
                z = 1;
            end
            yoffset = yrange* (-TextYMaxOffset +0.005*(ymaxoffset(ind)));
            
            textx = X(ind);
            texty = ymax-yoffset;
            if (isfield(InputStruct,'PlottingAxes'))
                textx = (textx-xmin)/xrange * Otherxrange + Otherxmin;
                texty = (texty-ymin)/yrange * Otheryrange + Otherymin;
            end
            
            text(textx,texty,'.','Color',Color,'FontSize',12,'FontWeight','bold','HorizontalAlignment','Center')
            ymaxoffset(ind) = ymaxoffset(ind)+1;
        end
        
%         Underflow
        f = find(Y(:,i)<ymin);
        for j = 1:length(f)
            ind = f(j);
            yoffset = yrange* (TextYMinOffset + 0.005*yminoffset(ind));
            
            textx = X(ind);
            texty = ymin+yoffset;
            if (isfield(InputStruct,'PlottingAxes'))
                textx = (textx-xmin)/xrange * Otherxrange + Otherxmin;
                texty = (texty-ymin)/yrange * Otheryrange + Otherymin;
            end
            
            text(textx,texty,'.','Color',Color,'FontSize',12,'FontWeight','bold')
            yminoffset(ind) = yminoffset(ind)+1;
        end
    end
    
end
