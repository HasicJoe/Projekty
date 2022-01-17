from re import U
from flask import Blueprint, render_template, jsonify, flash, Response

from iis_package.models import UserRoles
from .auth import user_privileges
from flask_login import login_required
import dataclasses as dc
import sqlalchemy as sa
import datetime as dt
from iis_package import db
personnel = Blueprint('personnel', __name__)

@dc.dataclass
class Unconfirmed_Reservation:
    id : int
    registered : str
    enter_stop : str
    exit_stop : str
    email : str
    seats : int


@personnel.route('/manage_tickets')
@user_privileges(user_role="personnel")
def manage_tickets():
    return render_template('personnel_views/manage_tickets.html')


@personnel.route('/load_reservations')
def load_reservations():
    from .models import Reservation, User
    data = Reservation.query.filter_by(confirmed=False, cancelled = False).all()

    reservations = list()
    for line in data:
        if not line.email:
            user = User.query.filter_by(id=line.user_id).first()
            reservations.append(Unconfirmed_Reservation(line.id, "registered", line.enter_stop, line.exit_stop, user.email, len(line.seats)))
        else:
            reservations.append(Unconfirmed_Reservation(line.id,"unregistered", line.enter_stop, line.exit_stop, line.email, len(line.seats)))
    return jsonify({ 'data' : reservations })


@personnel.route('/deny_reservation/<id>', methods=["GET"])
@user_privileges(user_role="personnel")
def deny_reservation(id):
    from .models import Reservation
    reservation = Reservation.query.filter_by(id=id).first()
    if not reservation:
        return Response(render_template('personnel_views/manage_tickets.html'), status=210)
    reservation.cancelled = True
    db.session.commit()
    return Response(render_template('personnel_views/manage_tickets.html'), status=200)


@personnel.route('/load_denied_reservations')
def load_denied_reservations():
    from .models import Reservation, User
    data = Reservation.query.filter_by(confirmed=False, cancelled = True).all()

    reservations = list()
    for line in data:
        if not line.email:
            user = User.query.filter_by(id=line.user_id).first()
            reservations.append(Unconfirmed_Reservation(line.id, "registered", line.enter_stop, line.exit_stop, user.email, len(line.seats)))
        else:
            reservations.append(Unconfirmed_Reservation(line.id,"unregistered", line.enter_stop, line.exit_stop, line.email, len(line.seats)))
    return jsonify({ 'data' : reservations })


@personnel.route('/load_confirmed_reservations')
def load_confirmed_reservations():
    from .models import Reservation, User
    data = Reservation.query.filter_by(confirmed=True, cancelled = False).all()

    reservations = list()
    for line in data:
        if not line.email:
            user = User.query.filter_by(id=line.user_id).first()
            reservations.append(Unconfirmed_Reservation(line.id, "registered", line.enter_stop, line.exit_stop, user.email, len(line.seats)))
        else:
            reservations.append(Unconfirmed_Reservation(line.id,"unregistered", line.enter_stop, line.exit_stop, line.email, len(line.seats)))
    return jsonify({ 'data' : reservations })


@personnel.route('/allow_reservation/<id>', methods=["GET"])
@user_privileges(user_role="personnel")
def allow_reservation(id):
    from .models import Reservation
    reservation = Reservation.query.filter_by(id=id).first()
    if not reservation:
        return Response(render_template('personnel_views/manage_tickets.html'), status=210)
    reservation.confirmed = True
    db.session.commit()
    return Response(render_template('personnel_views/manage_tickets.html'), status=200)

@personnel.route('/update_connection_location')
@user_privileges(user_role="personnel") 
def update_link_positions():
    return render_template('personnel_views/update_connection_location.html')

@personnel.route('/get_current_connection_info')
@user_privileges(user_role="personnel") 
def get_current_connection_info(): 
    from .models import Link

    curr_con = Link.query.all()
    today_interval = 1 << dt.date.today().weekday()
    curr_con = [ c for c in curr_con if c.interval & today_interval ]
        
    return jsonify({'data' : curr_con })
    
@personnel.route('/get_connection_data/<connection_id>')
@user_privileges(user_role="personnel")
def get_connection_data(connection_id):
    from .models import Link, Route, Current_location

    connection = Link.query.get(connection_id)
    if not connection:
        return {'success' : False, 'msg' : 'Internal server error', 'type' : 'danger'}

    # get routes for this connection
    routes = Route.query.filter_by(link_id=connection_id).all()
    if not routes:
        return {'success' : False, 'msg' : 'Internal server error', 'type' : 'danger'}

    # get current location if specified
    location = Current_location.query.filter_by(connection_id=connection_id).first()

    # location was not yet specified for connection
    index = location.location if location else -1
    html_data = ""

    # button for updating current location
    html_data += f"<button type='button' class='col-12 btn btn-lg btn-primary mb-5' onclick='update_curr_location(\"{index}\", \"{connection_id}\");'>Update current location to the next stop</button>"

    # wrapper for routes
    html_data += "<div class='col-12 d-inline-block border rounded-3 p-3 m-1'>"

    for i, route in enumerate(routes):
        if i == index:
            html_data += f"<p><code class='me-3'>{route.arrival_time.strftime('%H:%M')} -"\
                f" {route.departure_time.strftime('%H:%M')}</code>{route.stop_id}<h3 class='ms-5'>(Current location)</h3></p>"
        else:
            html_data += f"<p><code class='me-3'>{route.arrival_time.strftime('%H:%M')} -"\
                f" {route.departure_time.strftime('%H:%M')}</code>{route.stop_id}</p>"

    html_data += "</div>"

    return {'success' : True, 'html_content' : f"<div class='row m-3'>{html_data}</div>" }

@personnel.route('/update_curr_location/<index>/<connection_id>')
@user_privileges(user_role="personnel")
def update_curr_location(index, connection_id):

    from .models import Link, Current_location

    connection = Link.query.get(connection_id)
    if not connection:
        return {'success' : False, 'msg' : 'Internal server error', 'type' : 'danger'}

    # index does not exist (we will create it)
    if int(index) == -1:
        old_loc = Current_location.query.filter_by(connection_id=connection_id).all()
        if old_loc:
            return {'success' : False, 'msg' : 'Internal server error : undefined location left in the system', 'type' : 'danger'}
        
        # create new location
        location = Current_location(
            location=0,
            connection_id=connection_id
        )

        db.session.add(location)
        db.session.commit()
    elif int(index) >= len(connection.routes)-2:
        # we reach last stop of the connection (kill current connection)
        Current_location.query.filter_by(connection_id=connection_id).delete()
        db.session.commit()
    else:
        # move index of location by one
        location = Current_location.query.filter_by(connection_id=connection_id).first()
        location.location += 1
        db.session.commit()

    return {'success' : True, 'connection_id' : connection_id}
