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

	addEvents(segmentParent) {
		segmentParent.addEventListener('mouseover', (event) => {
			const hoverInfo = document.getElementById('hoverInfo');
			hoverInfo.innerHTML = 'name: ' + this.name + ' / length: ' + this.length.toFixed(3)
				+ ' / ID: ' + this.ID
			hoverInfo.style.visibility='visible';
			this.segmentBody.classList.add('highlightSegment');
			this.segmentBody2.classList.add('highlightSegment');
		});
		segmentParent.addEventListener('mouseout', (event) => {
			const hoverInfo = document.getElementById('hoverInfo');
			this.segmentBody.classList.remove('highlightSegment');
			this.segmentBody2.classList.remove('highlightSegment');
			hoverInfo.style.visibility='hidden';
		});
	}

	findElementsV1(container) { /* < 5.23 */
		this.segmentBody = container.querySelector(`g polygon[id]#${this.name}`);
		this.segmentBody.style.fill = '';
		this.segmentBody2 = this.segmentBody.nextElementSibling;		this.segmentBody2.style.fill = '';
		const segmentParent = this.segmentBody.parentElement;
		this.segmentPath = segmentParent.querySelector('polyline');
		this.addEvents(segmentParent);
	}

	findElementsV2(container) { /* >= 5.23 */
		this.segmentBody = container.querySelector(`#pgsg_${this.name}`);
		this.segmentBody.style.fill = '';
		this.segmentBody2 = this.segmentBody.nextElementSibling;		this.segmentBody2.style.fill = '';
		const segmentParent = container.querySelector(`#gsg_${this.name}`);
		this.segmentPath = container.querySelector(`#plsg_${this.name}`);				
		this.addEvents(segmentParent);
	}

	xy(percentage) { /* get coords from percentage position */
		const linepos = (percentage / 100.0) * this.segmentPath.getTotalLength();
		return this.segmentPath.getPointAtLength(linepos);
	}

	paintShuttle(svg) {
		this.segmentPath.parentElement.appendChild(svg);
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
	constructor(index, active, PLCopen, segment, pos ) {
		this.index = index;
		this.active = active;
		this.PLCopen = PLCopen;
		this.segment = segment;
		this.pos = pos; /* [%] */
	}

	plcOpenStatus() {
		return ['disabled', 'standstill', 'homing', 'stopping', 'discrete motion', 'continous motion',
			'synchronized motion', 'error stop', 'startup', 'invalid configuration'][this.PLCopen];
	}

	createSVG() {
		const { x, y } = this.segment.xy(this.pos);
		this.svg = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
		this.svg.classList.add('shuttle');
		this.svg.classList.add('shuttleId_' + this.index);
		this.svg.setAttribute('shuttle-id', this.index);
		this.svg.setAttribute('r', '0.012');
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
			const hoverInfo = document.getElementById('hoverInfo');
			const deleted = !this.active ? 'deleted shuttle !' : '';
			hoverInfo.innerHTML = deleted + 'index: ' + this.index + ' / ' + 'PLCopen: ' + this.plcOpenStatus() + ' / '
				+ 'segment: ' + this.segment.name + ' / '
				+ 'segment-position: ' + this.segment.segmentPosition(this.pos);
			this.svg.classList.add('highlightShuttle');
			hoverInfo.style.visibility='visible';
			event.stopPropagation();
		});
		this.svg.addEventListener('mouseout', (event) => {
			const hoverInfo = document.getElementById('hoverInfo');
			this.svg.classList.remove('highlightShuttle');
			hoverInfo.style.visibility='hidden';
		});
	}

	paint() {
		this.segment.paintShuttle(this.svg);
	}
}


class Assembly {

	constructor() {
		this.segment = [];
		this.offline = true;
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
		let request = fetch('/TrakWebApi/segments');
		let response = await request;
		let segmentInfo = await response.json()
		segmentInfo.forEach(item => this.segment.push(new Segment(item.ID, item.name, item.length)));

		/* load SvgData */
		const svgParent = document.createElement('div');
		svgParent.setAttribute('id', 'svgParent');
		request = fetch('/TrakWebApi/svgdata');
		response = await request;
		if (response.status !== 200)
			throw new Error(`Error fetching ${response.url}`);
		svgParent.innerHTML = await response.text();
		const svg = svgParent.querySelector('svg');

		const workspace = svgParent.querySelector('#workspace');
		const container = workspace.parentElement;
		const segmentObjects = container.querySelector('#segments');
		if (segmentObjects) { /* >= 5.23 */
			segmentObjects.querySelector('#sg_legend').remove();
			container.querySelector('#sectors').remove();
			container.querySelector('#barriers').remove();
			container.querySelector('#workspace').remove();
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
		svg.viewBox.baseVal.x = svg.getBBox().x-0.02;
		svg.viewBox.baseVal.width = svg.getBBox().width+0.04;
		svg.viewBox.baseVal.y = svg.getBBox().y-0.02;
		svg.viewBox.baseVal.height = svg.getBBox().height+0.04;
		hoverInfo.style.visibility='hidden';
	}

	removeAllShuttles = () => {
		const shuttles = document.querySelectorAll('.shuttle');
		if (shuttles.length != 0) {
			shuttles.forEach((s) => s.parentNode.removeChild(s));
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
				this.removeAllShuttles();
				if(monitor.length){
					monitor.forEach( s => {
						const first = s[0]; 
						const second = s[1];
						const index = first;
						const plcOpen = second & 0x7f;
						const active = !!(second & 0x100);
						const virtual = !!(second & 0x200);
						const segmentIndex = (second >> 10) & 0x7ff;
						const segmentPosition = (second >> 21) & 0x7f;
						const segment = this.segment[segmentIndex];
						const shuttle = new Shuttle( index, active, plcOpen, segment, segmentPosition);
						shuttle.createSVG();
						shuttle.paint();
					});
				}
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

}


document.addEventListener('DOMContentLoaded', async () => {
	const assembly = new Assembly();
	if( await assembly.load() ){
		await assembly.cyclicRefresh();
	}
})
