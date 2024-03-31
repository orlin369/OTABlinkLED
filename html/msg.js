// function compareArrays(a, b)
// {
// 	console.log(a);
// 	console.log(b);
// 	return JSON.stringify(a) === JSON.stringify(b);
// }

var loadedMessages = [];

function compareArrays (a, b)
{
	if (a.length !== b.length)
	{
		return false;
	}
	
	var sameCount = 0;
	// Comparing each element of your array
	for (var i = 0; i < a.length; i++)
	{
		if ((a[i] !== null) && (b[i] !== null))
		{
			var sameMsg = a[i]["msg"] === b[i]["msg"];
			var sameNox = a[i]["nox"] === b[i]["nox"];
			var sameClr = a[i]["color"] === b[i]["color"];

			console.log({"sameMsg": sameMsg,"nox": sameNox,"sameMsg": sameMsg});
			if ((sameMsg && sameNox && sameClr) == false)
			{
				sameCount++;
			}
		}
		else
		{
			sameCount++;
		}
	}
	if (sameCount == a.length)
	{
		return true;
	}
	
	return false;	
}

function dismissMessage(id)
{
	var dismissed = getDismissedIds();
	dismissed.push(id);
	
	if(dismissed.length > 1000)
	{
		dismissed.shift();
	}

	setDismissedIds(dismissed);
}

function isMessageDismissed(id)
{
	var dismissed = getDismissedIds();
	return (dismissed.indexOf(id) != -1);
}

function getDismissedIds()
{
	var dismissed_str = localStorage.getItem('dismissed');
	return dismissed_str ? JSON.parse(dismissed_str) : [];
}

function setDismissedIds(ids)
{
	localStorage.setItem('dismissed', JSON.stringify(ids));
}

function isMessageLoaded(id)
{
	return (loadedMessages.indexOf(id) != -1);
}

function loadMessages(cnt, messages)
{
	for(var i = 0; i < messages.length; i++)
	{
		addMessage(cnt, messages[i]);
	}
}

function addMessage(cnt, data)
{
	if (data === null)
	{
		return;
	}

	if(isMessageDismissed(data.id) || isMessageLoaded(data.id))
	{
		return;
	}

	var interval = null;
	var msgs = document.getElementById(cnt);
	var row = document.createElement('li');
	
	var msg_col = document.createElement('span');
	msg_col.appendChild(document.createTextNode(data.msg));
	msg_col.classList.add('msg-text');
	
	var time_col = document.createElement('span');
	time_col.appendChild(document.createTextNode(formatDt(data.dt)));
	time_col.classList.add('msg-time');
	
	var close_col = document.createElement('span');
	var close_col_button = document.createElement('button');
	close_col_button.appendChild(document.createTextNode('X'));
	if(!data.nox) close_col.appendChild(close_col_button);
	close_col.classList.add('msg-close');
	
	row.appendChild(msg_col);
	row.appendChild(time_col);
	row.appendChild(close_col);
	msgs.appendChild(row);
	loadedMessages.push(data.id);
	
	if(data.dt > 0)
	{
		interval = setInterval(function()
		{
			if(data.dt <= 1)
			{
				removeMessage(row, interval, data.id);
			}
			else
			{
				time_col.innerHTML = formatDt(--data.dt);
			}
			
		}, 1000);
	}
	
	close_col_button.addEventListener('click', function()
	{
		removeMessage(row, interval, data.id);
	});
	
	if(data.color)
	{
		row.style = 'background-color:' + data.color + ';';
	}
}

function removeMessage(row, interval, id)
{
	dismissMessage(id);
	clearInterval(interval);
	row.classList.add('hidden');
	
	setTimeout(function()
	{
		row.remove();
		
	}, 1000);
}

function formatDt(dt)
{
	if(dt == 0)
	{
		return '';
	}
	
	var minutes = 0;
	var seconds = dt;
	
	if(seconds >= 60)
	{
		minutes = Math.floor(seconds / 60);
		seconds = seconds - (minutes * 60);
	}
	
	minutes = minutes.toString();
	seconds = seconds.toString();
	
	if(minutes.length < 2)
	{
		minutes = '0' + minutes;
	}
	
	if(seconds.length < 2)
	{
		seconds = '0' + seconds;
	}
	
	return minutes + ':' + seconds;
}