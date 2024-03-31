class Chart {
    ctx;
    data;
    maxData;
    margin;
	footer;
    width;
    height;
    xStep;
    yMin;
    yMax;
    yScale;
	colors;
	
    constructor(element_name)
    {
        this.ctx = document.getElementById(element_name).getContext("2d");
		this.colors = ['#f32196', '#33ffac', '#33acff', '#ff3361', '#ffe933'];
		this.setData([]);
		this.setGeometry();
    }
    setData(data)
    {
		if(data.length > this.data_length)
		{
			data = data.slice(this.data_length * -1);
		}
		
        this.data = data;
    }
	setGeometry(data_length=50, y_min=-100, y_max=100, margin=25, footer=55)
	{
		this.footer = footer;
        this.margin = margin;
        this.width = this.ctx.canvas.width - this.margin * 2;
        this.height = this.ctx.canvas.height - this.margin * 2 - this.footer;

        this.data_length = data_length;
        this.xStep = this.width / this.data_length;

        this.yMin = y_min;
        this.yMax = y_max;
        this.yScale = this.height / (this.yMax - this.yMin);
	}
    setMaxSize(max_data)
    {
        this.data_length = max_data
        this.xStep = this.width / this.data_length;
    }
    clear()
    {
        this.ctx.clearRect(0, 0, this.ctx.canvas.width, this.ctx.canvas.height);
    }
    drawGrid()
    {
        this.ctx.strokeStyle = "#3A3A3A";
        this.ctx.lineWidth = 1;
        this.ctx.beginPath();
        for (var x = this.margin; x <= this.ctx.canvas.width - this.margin; x += this.xStep)
        {
            this.ctx.moveTo(x, this.margin);
            this.ctx.lineTo(x, this.ctx.canvas.height - this.margin - this.footer);
        }
        for (var y = this.margin; y <= this.ctx.canvas.height - this.margin - this.footer; y += this.yScale)
        {
            this.ctx.moveTo(this.margin, y);
            this.ctx.lineTo(this.ctx.canvas.width - this.margin, y);
        }
        this.ctx.stroke();

        this.ctx.strokeStyle = "#FFFFFF";
        this.ctx.lineWidth = 2;
        this.ctx.beginPath();
        var y = this.margin + this.height - (0 - this.yMin) * this.yScale;
        this.ctx.moveTo(this.margin, y);
        this.ctx.lineTo(this.ctx.canvas.width - this.margin, y);
        this.ctx.stroke();
    }
    drawLine()
    {
		if(this.data.length > 0 && this.data[0].constructor === Array)
		{
			var current = [];
			
			for(var i = 0; i < this.data.length; i++)
			{
				for(var j = 0; j < this.data[i].length; j++)
				{
					if(current[j] === undefined)
					{
						current[j] = [];
					}
					
					current[j].push(this.data[i][j]);
				}
			}
			
			for(var i = 0; i < current.length; i++)
			{
				if(i === 0)
				{
					this.drawLabels(current[i]);
				}
				else
				{
					this.drawSingleLine(current[i], i - 1);
				}
			}
		}
		else
		{
			this.drawSingleLine(this.data, 0);
		}
    }
	drawSingleLine(data, color)
	{
		this.ctx.strokeStyle = this.colors[color];
        this.ctx.lineWidth = 2;
        this.ctx.beginPath();
		
        for (var i = 0; i < data.length; i++)
        {
            var x = this.margin + i * this.xStep;
            var y = this.margin + this.height - (data[i] - this.yMin) * this.yScale;
			
            if (i == 0)
            {
                this.ctx.moveTo(x, y);
            }
            else
            {
                this.ctx.lineTo(x, y);
            }
        }
		
        this.ctx.stroke();
	}
	drawLabels(labels)
	{
		for (var i = 0; i < labels.length; i++)
        {
			this.ctx.save();
			var x = this.margin + i * this.xStep;
			this.ctx.fillStyle = "#ffffff";
			this.ctx.font = "10px sans-serif";
			this.ctx.translate(x, this.height + this.margin + this.footer);
			this.ctx.rotate(-Math.PI / 3);
			this.ctx.textAlign = 'left';
			this.ctx.fillText(labels[i], 0, -11);
			this.ctx.restore();
		}

	}
	update(data, max)
	{
		this.setData(data);
        this.clear();
        this.drawGrid();
        this.drawLine();
		
		if(max !== undefined)
		{
			this.setMaxSize(max);
		}
	}
}