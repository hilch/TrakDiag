"use strict";

/* @ */

class Segment {
	constructor(ID, name, length) {
		this.ID = ID;
		this.name = name;
		this.length = length;
		this.segmentPath = undefined;
		this.segmentBody = undefined;
		this.segmentBody2 = undefined;
	}

	showSegmentDialog = (event) => {
		const content = [
			['ID: ', this.ID],
			['Name: ', `"${this.name}"` ],
			['Length: ', `${this.length.toFixed(3)} m`],
		];
		assembly.showModalDialog( `Segment "${this.name}"`, content, event );
	}

	addEvents(segmentParent) {
		segmentParent.addEventListener('mouseover', (event) => {
			this.segmentBody.classList.add('highlightSegment');
			this.segmentBody2.classList.add('highlightSegment');
		});
		segmentParent.addEventListener('mouseout', (event) => {
			this.segmentBody.classList.remove('highlightSegment');
			this.segmentBody2.classList.remove('highlightSegment');
		});
		segmentParent.addEventListener('click', (event) => {
			this.showSegmentDialog(event);
		});
	}

	addTooltip(segmentParent){
		let tooltipContent = 'Segment name: "' + this.name + '" / length: ' + this.length.toFixed(3)
		+ ' / ID: ' + this.ID
		let tooltipNode = document.createElementNS('http://www.w3.org/2000/svg', 'title');
		tooltipNode.appendChild(document.createTextNode(tooltipContent));
		segmentParent.appendChild(tooltipNode);
	}


	findElementsV1(container) { /* < 5.23 */
		this.segmentBody = container.querySelector(`g polygon[id]#${this.name}`);
		this.segmentBody.style.fill = '';
		this.segmentBody2 = this.segmentBody.nextElementSibling;		this.segmentBody2.style.fill = '';
		const segmentParent = this.segmentBody.parentElement;
		this.segmentPath = segmentParent.querySelector('polyline');
		this.addEvents(segmentParent);
		this.addTooltip(segmentParent);
	}

	findElementsV2(container) { /* >= 5.23 */
		this.segmentBody = container.querySelector(`#pgsg_${this.name}`);
		this.segmentBody.style.fill = '';
		this.segmentBody2 = this.segmentBody.nextElementSibling;		this.segmentBody2.style.fill = '';
		const segmentParent = container.querySelector(`#gsg_${this.name}`);
		this.segmentPath = container.querySelector(`#plsg_${this.name}`);				
		this.addEvents(segmentParent);
		this.addTooltip(segmentParent);		
	}

	xy(percentage) { /* get coords from percentage position */
		const linepos = (percentage / 100.0) * this.segmentPath.getTotalLength();
		return this.segmentPath.getPointAtLength(linepos);
	}

	segmentPosition(percentage) {
		return ((percentage/100)*this.length).toFixed(3);
	}

	setStatus(flags) {
		const commReady = !!(flags &0x01);
		const ready = !!(flags & 0x02);
		const power = !!(flags & 0x04);
		const enable = !!(flags & 0x08);
		this.segmentBody.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline' );
		this.segmentBody2.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline' );
		if( commReady ){
			if( ready ){
				if( power ){
					this.segmentBody.classList.add('segReady');
					this.segmentBody2.classList.add('segReady');
				}
				else {
					this.segmentBody.classList.add('segDisabled');
					this.segmentBody2.classList.add('segDisabled');
				}
			}
			else {
				this.segmentBody.classList.add('segNotReadyForPowerOn');
				this.segmentBody2.classList.add('segNotReadyForPowerOn');
			}
		}
		else {
			this.segmentBody.classList.add('segOffline');
			this.segmentBody2.classList.add('segOffline');
		}
	}

}


class Shuttle {
	constructor(index, data ) {
		this.data = data;
		this.index = index;
		this.active = !!(data & 0x100);
		this.virtual = !!(data & 0x200);		
		this.PLCopen = data & 0x7f;
		const segmentIndex = (data >> 10) & 0x7ff;
		this.pos = (data >> 21) & 0x7f;
		this.segment = assembly.segment[segmentIndex];		
	}

	plcOpenStatus() {
		return ['disabled', 'standstill', 'homing', 'stopping', 'discrete motion', 'continous motion',
			'synchronized motion', 'error stop', 'startup', 'invalid configuration'][this.PLCopen];
	}

	addTooltip(svg) {
		let tooltipContent = `Shuttle ${ !this.active ? '(deleted !)' : ''} index: ${this.index}`;
		tooltipContent += ` / PLCopen: ${this.plcOpenStatus()} / segment: ${this.segment.name}`
		tooltipContent += ` / segment-position: ${this.segment.segmentPosition(this.pos)}`;	
		let tooltipNode = document.createElementNS('http://www.w3.org/2000/svg', 'title');
		tooltipNode.appendChild(document.createTextNode(tooltipContent));
		tooltipNode.classList.add('tooltiptext');
		svg.appendChild(tooltipNode);
	}

	async showShuttleDialog(event){
		try {
			let response = await fetch(`/TrakWebApi/shuttle?index=${this.index}`);
			let shuttle = await response.json()
			const title = `${!shuttle.active ? 'deleted ! ' : ''} Shuttle ${shuttle.index}`;
			const content = [
				['controlled: ', `${shuttle.controlled}`],
				['virtual: ', `${shuttle.virtual}`],				
				['User-ID: ', `"${shuttle.userID}"`],
				['PLCopen: ', shuttle.PLCopen],
				['Segment-Name: ', `"${shuttle.segmentName}"`],
				['Segment-Position: ', `${shuttle.segmentPosition.toFixed(3)} m`],
				['Sector-Name: ', `"${shuttle.sectorName}"`],
				['Sector-Position: ', `${shuttle.sectorPosition.toFixed(3)} m`]				
			];
			if( 'errorTexts' in shuttle ){
				for( let n = 0; n < shuttle.errorTexts.length; ++n ){
					const row = [`${new Date(shuttle.errorTexts[n].t).toISOString()}: `, shuttle.errorTexts[n].txt]
					content.push(row);
				}	
			}			
			assembly.showModalDialog( title, content, event );
		}
		catch( err ){
			console.log(err);
		}

	}

	createSVG() {
		const { x, y } = this.segment.xy(this.pos);
		this.svg = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
		this.svg.classList.add('shuttle');	
		this.svg.classList.add('shuttleId_' + this.index);
		this.svg.setAttribute('shuttle-id', this.index);
		this.svg.setAttribute('shuttle-data', this.data )
		this.svg.setAttribute('r', '0.024');
		this.svg.setAttribute('cx', x);
		this.svg.setAttribute('cy', y);
		switch (this.PLCopen) {
			case 0: /* disabled */
			case 8: /* startup */
				this.svg.classList.add('shuttleDisabled');
				break;

			case 1: /* standstill */
			case 2: /* homing */
			case 3: /* stopping */
			case 4: /* discrete motion */
			case 5: /* continuous motion */
			case 6: /* synchronized motion */
				this.svg.classList.add('shuttleReady');
				break;

			case 7: /* errorstop */
			case 9: /* invalid configuration */
				this.svg.classList.add('shuttleErrorStop');
				break;
		}
		if (!this.active) this.svg.classList.add('shuttleDeleted');
		this.svg.addEventListener('mouseover', (event) => {
			this.svg.classList.add('highlightShuttle');
			event.stopPropagation();
		});
		this.svg.addEventListener('mouseout', (event) => {
			this.svg.classList.remove('highlightShuttle');
		});
		this.svg.addEventListener('click', (event) => {
			this.showShuttleDialog(event);
			event.stopPropagation();
		});

		this.addTooltip(this.svg)
	}

	paint() {
		assembly.shuttleParent.appendChild(this.svg);
	}
}


class ShuttleList {

	constructor( monitor ) {
		const allShuttleNodes = assembly.shuttleParent.querySelectorAll('.shuttle');
		this.oldShuttles = new Map();
		for( let n = 0; n < allShuttleNodes.length; ++n ){
			const s = allShuttleNodes[n];
			this.oldShuttles.set( parseInt(s.getAttribute('shuttle-id')), parseInt(s.getAttribute('shuttle-data')));
		}
		this.newShuttles = new Map();
		for( let n = 0; n < monitor.length; ++n ){
			const s = monitor[n];
			const index = s[0];
			this.newShuttles.set(index, s[1]);
		}
	}

	/* shuttles that are to be deleted */
	toBeDeleted = () => {
		let result = new Set();
		this.oldShuttles.forEach( (value, key, map)=>{
			if( !this.newShuttles.has(key)) // shuttle does not exist anymore
				result.add(key);
		})
		this.newShuttles.forEach( (value, key, map)=>{
			if( this.oldShuttles.has(key) && value != this.oldShuttles.get(key)){  // shuttle data have changed
				result.add(key);
			}
		})
		return result;
	}

	/* shuttles that are to be painted new */
	toBeCreated = () => {
		let result = new Set();
		this.newShuttles.forEach( (value, key, map)=>{
			if( this.oldShuttles.get(key) != value ) // shuttle data have changed
				result.add(key);
		})
		return result;		
	}


	paint = () => {
		/* delete invalid shuttles */
		this.toBeDeleted().forEach( index =>{
			const node = assembly.shuttleParent.querySelector(`.shuttleId_${index}`);
			assembly.shuttleParent.removeChild(node);			
		});
		/* paint new and changed shuttles */
		this.toBeCreated().forEach( index =>{
			const shuttle = new Shuttle( index, this.newShuttles.get(index));
			shuttle.createSVG();
			shuttle.paint();
		});
	}
}


class Assembly {

	constructor() {
		this.segment = [];
		this.offline = true;
		this.shuttleParent = null;
	}

	wait = async (milliseconds) => {  /* wait x ms */
		const promise = new Promise(res => setTimeout(() => res(), milliseconds));
		return promise;
	}

	load = async () => {
		/* load segment data */
		const hoverInfo = document.getElementById('hoverInfo');
		hoverInfo.innerText='loading SVG...';
		hoverInfo.style.visibility='visible';
		try {
			let response = await fetch('/TrakWebApi/segments');
			let segmentInfo = await response.json()
			segmentInfo.forEach(item => this.segment[item.ID] = new Segment(item.ID, item.name, item.length));
		}
		catch( err ){
			document.getElementById('timeoutBox').style.display = 'block';
			this.offline = true;
			return false;
		}

		/* load SvgData */
		const svgParent = document.createElement('div');
		svgParent.setAttribute('id', 'svgParent');
		let response = undefined;
		try {
			response = await fetch('/TrakWebApi/svgdata');
			if (response.status !== 200)
				throw new Error(`Error fetching ${response.url}`);
			svgParent.innerHTML = await response.text();
			const svg = svgParent.querySelector('svg');

			const workspace = svgParent.querySelector('#workspace');
			const container = workspace.parentElement;
			const segmentObjects = container.querySelector('#segments');
			if (segmentObjects) { /* >= 5.23 */
				segmentObjects.querySelector('#sg_legend').remove();
				const sectors = container.querySelector('#sectors');
					if( sectors ) sectors.remove();
				const barriers = container.querySelector('#barriers');
					if( barriers ) barriers.remove();
				container.querySelector('#workspace').remove();
				const processpoints = container.querySelector('#processpoints');
					if( processpoints ) processpoints.remove();
				this.segment.forEach(s => s.findElementsV2(segmentObjects.querySelector('#sg_layout')));
			}
			else { /* < 5.23 */
				/* remove all sectors */
				container.querySelectorAll('#sector').forEach((e) => { e.remove(); })

				/* remove triggerpoints and barriers */
				container.querySelectorAll('#triggerpoint').forEach((e) => { e.remove(); })
				container.querySelectorAll('polyline[stroke="darkorange"]').forEach((e) => { e.remove(); })
				container.querySelectorAll('polyline[stroke="purple"]').forEach((e) => { e.remove(); })
				/* remove all legend tables */
				svgParent.querySelectorAll('text').forEach((e) => {
					if (e.innerHTML == ' Sectors' || e.textContent == ' Segments')
						e.parentElement.parentElement.remove();
					else if (/Process\s*points/.test(e.innerHTML))
						e.parentElement.parentElement.remove();
				})
				workspace.remove();
				this.segment.forEach(s => s.findElementsV1(container));
			}
			document.querySelector('#svgParent').replaceWith(svgParent);
			this.shuttleParent = document.createElementNS('http://www.w3.org/2000/svg', 'g');
			this.shuttleParent.setAttribute('id', 'shuttles');
			svg.appendChild(this.shuttleParent);
			svg.viewBox.baseVal.x = svg.getBBox().x-0.05;
			svg.viewBox.baseVal.width = svg.getBBox().width+0.1;
			svg.viewBox.baseVal.y = svg.getBBox().y-0.05;
			svg.viewBox.baseVal.height = svg.getBBox().height+0.1;
			hoverInfo.style.visibility='hidden';
			panzoom( svg );
			return true;
		}
		catch( err ){
			document.getElementById('timeoutBox').style.display = 'block';	
			this.offline = true		
			return false;
		}
	}


	/* read shuttle positions */
	async readShuttlePositions() {
		try {
			const res = await fetch('/TrakWebApi/positions', { signal: AbortSignal.timeout(5000) });
			if (res.status === 200) {
				if( this.offline ) document.querySelector('#timeoutBox').style.display = 'none';
				this.offline = false;
				const monitor = await res.json();
				//this.removeAllShuttles();
				if(monitor.length){
					const shuttleList = new ShuttleList( monitor );
					shuttleList.paint();
				}
				this.monitorOld = monitor;
			}
		} catch( err ){
			if( err.name == 'AbortError' ){;
				this.offline = true;
				document.querySelector('#timeoutBox').style.display = 'block';
			}
		}
	}

	/* read segment flags */
	async readSegmentFlags() {
		try {
			const res = await fetch('/TrakWebApi/segment_status');
			if (res.status === 200) {
				const flags = await res.json();
				this.segment.forEach( (s,i) => s.setStatus(flags[i]) );
			}
		} catch(err){}
	}

	/* cyclic refresh */
	async cyclicRefresh(assembly) {
		let cycles = 10;
		while (true) {
			++cycles;
			if( (cycles >= 10) && !this.offline ) {
				cycles = 0;
				await this.readSegmentFlags()
			}
			await this.readShuttlePositions(assembly);
			await this.wait(50);
		}
	}


	/* modal dialog */
	showModalDialog = (title, content, event) => {	
		const dlg = document.getElementById('modalDialog');
		document.getElementById('modalDialogTitle').textContent = title;
		const modalDialogContent = document.getElementById('modalDialogContent');
		modalDialogContent.innerHTML='';
		const tbl = document.createElement("table");
		const tblBody = document.createElement("tbody");
    	// Create all cells
		for (let r = 0; r < content.length; r++) {
			// Create a table row
			const row = document.createElement("tr");
			for (let c = 0; c < 2; c++) {
				// Create a <td> element and a text node
				const cell = document.createElement("td");
				const cellText = document.createTextNode(content[r][c], content[r][c]);
				cell.appendChild(cellText);
				row.appendChild(cell);
			}
			// Add the row to the end of the table body
			tblBody.appendChild(row);
		}
		// Put the <tbody> in the <table>
		tbl.appendChild(tblBody);
	    // Append <table> into modalDialogContent>
		modalDialogContent.appendChild(tbl);
		dlg.show();
		const dlgRect = dlg.getBoundingClientRect();
		const svgRect = document.getElementById('svgParent').getBoundingClientRect();
		const maxWidth = svgRect.width*0.9;
		if( dlgRect.width > maxWidth )
			dlg.style.width = maxWidth + 'px';

		let top = event.pageY + 10;
		if( ((top + dlgRect.height) > svgRect.y + svgRect.height) || 
			(event.pageY > (svgRect.height+svgRect.y)) ){
			top = event.pageY - 5 - dlgRect.height;
		}
		if( top < 5 ) top = 5;
		
		let left = event.pageX - 10;
		if((left + dlgRect.width+5) > (svgRect.x + svgRect.width)){
			left = svgRect.x + svgRect.width-dlgRect.width-5;
		}
		if( left < 5 ) left = 5;

		dlg.style.top = `${top}px`;
		dlg.style.left = `${left}px`;


	}
}


document.addEventListener('DOMContentLoaded', async () => {
	this.assembly = new Assembly();
	if( await assembly.load() ){
		await assembly.cyclicRefresh();
	}
})
